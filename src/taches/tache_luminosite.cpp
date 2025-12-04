/**
 * @file tache_luminosite.cpp
 * @brief Implémentation de la tâche de gestion du capteur de luminosité Grove Light Sensor.
 * 
 * Ce fichier définit une tâche FreeRTOS qui lit les données du capteur de luminosité
 * Grove Light Sensor (basé sur une photorésistance) connecté sur une entrée analogique.
 * La valeur brute est convertie en lux approximatifs pour l'affichage.
 * 
 * Le Grove Light Sensor utilise une photorésistance (LDR) et fournit une tension
 * proportionnelle à la luminosité ambiante.
 */

#include "taches/tache_luminosite.h"
#include "configuration.h"

// Variable globale pour stocker la valeur de luminosité en lux (approximatif)
volatile float luminosite = 0;

// Nom du capteur
const char* Nom_luminosite = "Lum";

/**
 * @brief Convertit la valeur analogique en lux approximatifs.
 * 
 * Le Grove Light Sensor V1.2 utilise un phototransistor LS06-S 
 * avec un ampli-op LM358 en suiveur de tension.
 * 
 * La tension de sortie est directement proportionnelle à la luminosité:
 * - Plus de lumière = tension plus élevée = ADC plus élevé
 * - Obscurité = tension proche de 0V = ADC proche de 0
 * 
 * Le phototransistor LS06-S a une sensibilité d'environ 0.4-0.8 mA 
 * à 1000 lux. Avec R = 68kΩ, cela donne environ 27-54mV par lux
 * en première approximation (non linéaire en réalité).
 * 
 * @param analogValue Valeur brute de l'ADC (0-4095 pour ESP32-S2)
 * @return Luminosité approximative en lux
 */
float convertToLux(int analogValue) {
    // Conversion de la valeur ADC en tension (ESP32-S2: 0-3.3V, ADC 12 bits)
    float voltage = (analogValue / 4095.0f) * 3.3f;
    
    // Le phototransistor LS06-S avec R=68k donne environ:
    // - 0V à l'obscurité
    // - ~0.5V à 100 lux (éclairage intérieur faible)
    // - ~1.5V à 500 lux (bureau bien éclairé)
    // - ~3.0V à 1000+ lux (lumière vive)
    //
    // Facteur de conversion ajustable selon la sensibilité souhaitée
    
    float lux = voltage * 100.0f;
    
    // Limiter les valeurs
    if (lux > 10000.0f) lux = 10000.0f;
    if (lux < 0.0f) lux = 0.0f;
    
    return lux;
}

/**
 * @brief Tâche FreeRTOS dédiée à la gestion du capteur de luminosité.
 * 
 * Cette tâche lit la valeur analogique du capteur Grove Light Sensor,
 * la convertit en lux approximatifs et met à jour la variable globale `luminosite`.
 * La tâche est configurée pour se mettre en veille pendant un certain intervalle 
 * de temps (défini par `LUMINOSITE_INTERVAL_MS`) avant de mettre à jour à nouveau la valeur.
 * 
 * @param pvParameters Paramètre non utilisé, requis par le prototype de la fonction FreeRTOS.
 */
void tache_luminosite(void *pvParameters)
{
    // Configuration de l'ADC pour une meilleure résolution
    analogReadResolution(12);  // ESP32-S2 supporte 12 bits (0-4095)
    // Configurer l'atténuation pour la broche spécifique (plage 0-3.3V)
    analogSetPinAttenuation(LUMINOSITE_PIN, ADC_11db);
    
    Serial.print("Capteur de luminosite initialise sur GPIO");
    Serial.println(LUMINOSITE_PIN);
    
    // Test de lecture initiale pour diagnostic
    int testRead = analogRead(LUMINOSITE_PIN);
    float testVoltage = (testRead / 4095.0f) * 3.3f;
    Serial.print("Lecture initiale - ADC brut: ");
    Serial.print(testRead);
    Serial.print(" | Tension: ");
    Serial.print(testVoltage, 3);
    Serial.println(" V");
    
    // Délai initial pour stabilisation
    delay(500);
    
    // Boucle infinie de la tâche
    for (;;)
    {
        // Lecture de la valeur analogique (moyenne de plusieurs lectures pour stabilité)
        long somme = 0;
        const int nbLectures = 10;
        
        for (int i = 0; i < nbLectures; i++) {
            somme += analogRead(LUMINOSITE_PIN);
            delay(10);  // Petit délai entre les lectures
        }
        
        int valeurMoyenne = somme / nbLectures;
        
        // Calcul de la tension pour debug
        float voltage = (valeurMoyenne / 4095.0f) * 3.3f;
        
        // Conversion en lux
        float lux = convertToLux(valeurMoyenne);
        
        // Mise à jour de la variable globale
        luminosite = lux;
        
        // Affichage de debug détaillé
        Serial.print("Lum - ADC: ");
        Serial.print(valeurMoyenne);
        Serial.print(" | V: ");
        Serial.print(voltage, 3);
        Serial.print("V | Lux: ");
        Serial.print(lux, 1);
        Serial.println(" lx");
        
        // Temporisation de la tâche avant la prochaine mesure
        vTaskDelay(LUMINOSITE_INTERVAL_MS / portTICK_PERIOD_MS);
    }
}
