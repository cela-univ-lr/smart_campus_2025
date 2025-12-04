 /**
 * @file tache_tempHum.cpp
 * @brief Implémentation de la tâche de gestion du capteur DHT22 (température et humidité).
 * 
 * Ce fichier définit une tâche FreeRTOS qui lit les données du capteur DHT22.
 * Les valeurs de température et d'humidité sont mises à jour à intervalles réguliers 
 * et peuvent être utilisées par d'autres tâches (comme l'affichage OLED).
 * 
 * Un sémaphore est utilisé pour signaler aux autres tâches (comme CO2) que les 
 * premières mesures valides sont disponibles.
 */

#include "taches/tache_tempHum.h"
#include "variablesGlobales.h"
#include <DHT.h>

// Initialisation du capteur DHT22
DHT dht(TEMPHUM_PIN, DHTTYPE);

// Déclaration des variables globales pour stocker température et humidité
volatile float temperature;
volatile float humidite;

// Noms des capteurs (peuvent être utilisés pour l'affichage ou le débogage)
const char* Nom_temperature = "Temp"; 
const char* Nom_humidite = "Hum";

// Sémaphore pour signaler que les premières mesures sont disponibles
SemaphoreHandle_t xSemaphoreTempHumReady = NULL;

// Indicateur pour savoir si le sémaphore a déjà été signalé
static bool firstMeasureDone = false;

/**
 * @brief Tâche FreeRTOS dédiée à la gestion du capteur DHT22.
 * 
 * Cette tâche lit les valeurs de température et d'humidité depuis le capteur DHT22
 * et met à jour les variables globales `temperature` et `humidite`. 
 * La tâche est configurée pour se mettre en veille pendant un certain intervalle 
 * de temps (défini par `TEMPHUM_INTERVAL_MS`) avant de mettre à jour à nouveau les valeurs.
 * 
 * @param pvParameters Paramètre non utilisé, requis par le prototype de la fonction FreeRTOS.
 */
void tache_tempHum(void *pvParameters)
{ 
    // Création du sémaphore binaire (initialement non disponible)
    xSemaphoreTempHumReady = xSemaphoreCreateBinary();
    if (xSemaphoreTempHumReady == NULL) {
        Serial.println("Erreur: Impossible de creer le semaphore TempHumReady !");
    }
    
    // Initialisation du capteur DHT22
    dht.begin();
    Serial.println("DHT22 initialisé sur la broche 38");
    
    // Délai initial pour s'assurer que les initialisations du système sont terminées
    delay(2000); // DHT22 nécessite un délai initial

    // Boucle infinie de la tâche
    for (;;)
    {
        // Lecture de la température et de l'humidité depuis le DHT22
        float temp = dht.readTemperature();
        float hum = dht.readHumidity();
        
        // Vérification si les lectures sont valides
        if (!isnan(temp) && !isnan(hum)) {
            temperature = temp;
            humidite = hum;
            Serial.print("DHT22 - Température: ");
            Serial.print(temp);
            Serial.print(" °C | Humidité: ");
            Serial.print(hum);
            Serial.println(" %");
            
            // Signaler le sémaphore une seule fois après la première mesure valide
            if (!firstMeasureDone && xSemaphoreTempHumReady != NULL) {
                xSemaphoreGive(xSemaphoreTempHumReady);
                firstMeasureDone = true;
                Serial.println("DHT22 - Premiere mesure valide, semaphore signale !");
            }
        } else {
            Serial.println("DHT22 - Erreur de lecture du capteur");
        }

        // Temporisation de la tâche avant la prochaine mesure (en millisecondes)
        vTaskDelay(TEMPHUM_INTERVAL_MS / portTICK_PERIOD_MS);
    }
}
