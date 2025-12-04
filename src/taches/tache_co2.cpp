/**
 * @file tache_co2.cpp
 * @brief Implémentation de la tâche de gestion du capteur SGP30 (CO2 et TVOC).
 * 
 * Ce fichier définit une tâche FreeRTOS qui lit les valeurs de CO2 équivalent (eCO2) et 
 * de composés organiques volatils totaux (TVOC) depuis un capteur SGP30 via I2C.
 * Les valeurs sont mises à jour à intervalles réguliers et peuvent être utilisées par 
 * d'autres tâches (comme l'affichage OLED ou l'envoi via MQTT).
 * 
 * La compensation de l'humidité est effectuée à partir des données du capteur DHT22
 * pour améliorer la précision des mesures de eCO2 et TVOC.
 */

#include "taches/tache_co2.h"
#include <Wire.h>

// Instance du capteur SGP30
Adafruit_SGP30 sgp30;

// Variable globale pour stocker la valeur de CO2 équivalent (eCO2 en ppm)
volatile uint16_t co2 = 0;

// Variable globale pour stocker la valeur de TVOC (en ppb)
volatile uint16_t tvoc = 0;

// Nom du capteur CO2
const char* Nom_co2 = "CO2"; // Capteur de CO2 (SGP30)

// Indicateur d'état du capteur
static bool sgp30_ok = false;

/**
 * @brief Calcule l'humidité absolue à partir de la température et de l'humidité relative.
 * 
 * Cette fonction convertit l'humidité relative (%) et la température (°C) en humidité
 * absolue (g/m³), puis la formate au format attendu par le SGP30 (8.8 fixed point).
 * 
 * Formule utilisée: humidité absolue = (humidité relative / 100) * 6.112 * exp((17.62 * T) / (243.12 + T)) * 216.7 / (273.15 + T)
 * 
 * @param temperature Température en degrés Celsius
 * @param humidite Humidité relative en pourcentage (0-100%)
 * @return Humidité absolue au format 8.8 fixed point pour le SGP30
 */
uint32_t getAbsoluteHumidity(float temperature, float humidite) 
{
    // Constantes pour le calcul de la pression de vapeur saturante (formule de Magnus)
    const float A = 17.62f;
    const float B = 243.12f;
    
    // Calcul de l'humidité absolue en g/m³
    // Formule: AH = (RH/100) * 6.112 * exp((A*T)/(B+T)) * 216.7 / (273.15+T)
    float absHumidity = (humidite / 100.0f) * 6.112f * 
                        exp((A * temperature) / (B + temperature)) * 
                        216.7f / (273.15f + temperature);
    
    // Conversion au format 8.8 fixed point (8 bits entiers, 8 bits décimaux)
    // Multiplier par 256 pour décaler de 8 bits
    uint32_t absoluteHumidity = (uint32_t)(absHumidity * 256.0f);
    
    // Limiter la valeur maximale (le SGP30 accepte max 255.99 g/m³)
    if (absoluteHumidity > 0xFFFF) {
        absoluteHumidity = 0xFFFF;
    }
    
    return absoluteHumidity;
}

/**
 * @brief Initialise le capteur SGP30.
 * 
 * Cette fonction doit être appelée avant de démarrer la tâche tache_co2.
 * Elle initialise la communication I2C avec le capteur SGP30 et vérifie sa présence.
 * 
 * @return true si l'initialisation a réussi, false sinon.
 */
bool initSGP30()
{
    // Note: Wire.begin(8, 9) est appelé UNE SEULE FOIS dans main.cpp setup()
    // Ne jamais réappeler Wire.begin() ici !
    
    // Scan I2C pour diagnostic
    Serial.println("Scan I2C en cours...");
    byte count = 0;
    for (byte i = 1; i < 127; i++) {
        Wire.beginTransmission(i);
        if (Wire.endTransmission() == 0) {
            Serial.print("Peripherique I2C trouve a l'adresse 0x");
            if (i < 16) Serial.print("0");
            Serial.println(i, HEX);
            count++;
        }
    }
    Serial.print("Nombre de peripheriques I2C trouves: ");
    Serial.println(count);
    
    // Délai pour stabiliser le bus I2C après le scan
    delay(500);
    
    // Test de communication directe avec le SGP30 à l'adresse 0x58
    Wire.beginTransmission(0x58);
    byte error = Wire.endTransmission();
    if (error != 0) {
        Serial.print("Erreur communication I2C avec SGP30: ");
        Serial.println(error);
        sgp30_ok = false;
        return false;
    }
    Serial.println("Communication I2C avec SGP30 OK (0x58)");
    
    // Réinitialiser Wire avec les bonnes broches avant sgp30.begin()
    // car la bibliothèque Adafruit peut appeler Wire.begin() sans paramètres
    Wire.end();
    delay(100);
    Wire.begin(8, 9);
    delay(100);
    
    // Tentative d'initialisation du capteur SGP30
    if (!sgp30.begin()) {
        Serial.println("Erreur: sgp30.begin() a echoue !");
        Serial.println("Tentative avec Wire explicite...");
        
        // Réessayer avec Wire explicite
        Wire.end();
        delay(100);
        Wire.begin(8, 9);
        delay(100);
        
        if (!sgp30.begin(&Wire)) {
            Serial.println("Erreur: Capteur SGP30 non initialise !");
            sgp30_ok = false;
            return false;
        }
    }
    
    Serial.print("SGP30 trouvé - Numéro de série: ");
    Serial.print(sgp30.serialnumber[0], HEX);
    Serial.print(sgp30.serialnumber[1], HEX);
    Serial.println(sgp30.serialnumber[2], HEX);
    
    // Initialisation de la mesure de qualité de l'air (IAQ)
    // Le capteur nécessite environ 15 secondes de chauffe pour des mesures fiables
    // et 12 heures pour une calibration optimale
    if (!sgp30.IAQinit()) {
        Serial.println("Erreur: Echec de l'initialisation IAQ du SGP30 !");
        sgp30_ok = false;
        return false;
    }
    
    Serial.println("SGP30 initialise avec succes !");
    sgp30_ok = true;
    return true;
}

/**
 * @brief Tâche FreeRTOS dédiée à la gestion du capteur SGP30.
 * 
 * Cette tâche lit les valeurs de CO2 équivalent (eCO2) et de TVOC depuis le capteur SGP30
 * et met à jour les variables globales `co2` (eCO2) et `tvoc`.
 * La compensation de l'humidité est effectuée à partir des données du DHT22 pour
 * améliorer la précision des mesures.
 * La tâche est configurée pour se mettre en veille pendant un certain intervalle 
 * de temps (défini par `CO2_INTERVAL_MS`) avant de mettre à jour à nouveau les valeurs.
 * 
 * Note: Le SGP30 doit être interrogé au moins toutes les secondes pour maintenir
 * l'algorithme de calibration interne.
 * 
 * @param pvParameters Paramètre non utilisé, requis par le prototype de la fonction FreeRTOS.
 */
void tache_co2(void *pvParameters)
{
    // Attendre que la tâche DHT22 ait effectué sa première mesure valide
    Serial.println("CO2 - Attente des donnees temperature/humidite du DHT22...");
    
    // Attendre que le sémaphore soit créé par la tâche DHT22
    while (xSemaphoreTempHumReady == NULL) {
        Serial.println("CO2 - Attente de la creation du semaphore...");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    
    // Attendre le sémaphore avec un timeout de 30 secondes
    if (xSemaphoreTake(xSemaphoreTempHumReady, pdMS_TO_TICKS(30000)) == pdTRUE) {
        Serial.println("CO2 - Donnees DHT22 disponibles, demarrage des mesures CO2");
        // Redonner le sémaphore pour d'autres tâches éventuelles
        xSemaphoreGive(xSemaphoreTempHumReady);
    } else {
        Serial.println("CO2 - Timeout en attendant DHT22, demarrage quand meme...");
    }
    
    // Initialisation du SGP30 après avoir reçu les données du DHT22
    initSGP30();
    
    // Appliquer immédiatement la compensation d'humidité avec les valeurs du DHT22
    if (sgp30_ok && !isnan(temperature) && !isnan(humidite)) {
        uint32_t absHumidity = getAbsoluteHumidity(temperature, humidite);
        sgp30.setHumidity(absHumidity);
        Serial.print("CO2 - Compensation initiale appliquee: T=");
        Serial.print(temperature);
        Serial.print("C, H=");
        Serial.print(humidite);
        Serial.print("%, AH=");
        Serial.print(absHumidity / 256.0f, 2);
        Serial.println(" g/m3");
    }
    
    // Compteur pour la calibration et la mise à jour de l'humidité
    static uint32_t counter = 0;
    
    // Boucle infinie de la tâche
    for (;;)
    {
        if (sgp30_ok) {
            // Compensation de l'humidité toutes les 10 mesures (environ 10 secondes)
            // Utilise les valeurs de température et humidité du DHT22
            if (counter % 10 == 0) {
                // Vérifier que les valeurs du DHT22 sont valides
                if (!isnan(temperature) && !isnan(humidite) && 
                    temperature > -40.0f && temperature < 85.0f &&
                    humidite >= 0.0f && humidite <= 100.0f) {
                    
                    // Calcul de l'humidité absolue au format 8.8 fixed point
                    uint32_t absHumidity = getAbsoluteHumidity(temperature, humidite);
                    
                    // Envoi de la compensation au SGP30
                    sgp30.setHumidity(absHumidity);
                    
                    Serial.print("SGP30 - Compensation humidite appliquee: T=");
                    Serial.print(temperature);
                    Serial.print("C, H=");
                    Serial.print(humidite);
                    Serial.print("%, AH=");
                    Serial.print(absHumidity / 256.0f, 2);
                    Serial.println(" g/m3");
                }
            }
            
            // Lecture des mesures IAQ (eCO2 et TVOC)
            if (sgp30.IAQmeasure()) {
                // Mise à jour des variables globales avec les nouvelles valeurs
                co2 = sgp30.eCO2;  // eCO2 en ppm (400-60000)
                tvoc = sgp30.TVOC;      // TVOC en ppb (0-60000)
                
                // Affichage de debug (peut être désactivé en production)
                Serial.print("eCO2: ");
                Serial.print(co2);
                Serial.print(" ppm\t TVOC: ");
                Serial.print(tvoc);
                Serial.println(" ppb");
            } else {
                Serial.println("Erreur de lecture du SGP30 !");
            }
            
            // Optionnel: Récupération des valeurs brutes (H2 et Ethanol)
            // Utile pour le diagnostic et la calibration
            if (counter % 30 == 0) {  // Toutes les 30 mesures
                if (sgp30.IAQmeasureRaw()) {
                    Serial.print("H2 brut: ");
                    Serial.print(sgp30.rawH2);
                    Serial.print(" \t Ethanol brut: ");
                    Serial.println(sgp30.rawEthanol);
                }
            }
            
            counter++;
        } else {
            // Si le capteur n'est pas initialisé, afficher un message d'erreur
            Serial.println("SGP30 non initialise - tentative de reinitialisation...");
            initSGP30();
        }

        // Temporisation de la tâche avant la prochaine mesure
        // Note: Le SGP30 recommande une lecture toutes les secondes pour la calibration
        vTaskDelay(CO2_INTERVAL_MS / portTICK_PERIOD_MS);
    }
}
