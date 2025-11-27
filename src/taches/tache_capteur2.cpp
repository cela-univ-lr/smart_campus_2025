/**
 * @file tache_capteur2.cpp
 * @brief Implémentation de la tâche de gestion du capteur 2.
 * 
 * Ce fichier définit une tâche FreeRTOS qui simule la lecture d'un capteur (par exemple, un volume).
 * La valeur du capteur est mise à jour à intervalles réguliers et peut être utilisée par d'autres tâches (comme l'affichage OLED ou l'envoi via MQTT).
 */

#include "taches/tache_capteur2.h"

// Déclaration de la variable globale pour stocker la valeur du capteur 2 (exemple : pression)
volatile float capteur2;

// Nom du capteur 2 (peut être utilisé pour l'affichage ou le débogage)
const char* Nom_capteur2 = "Pression"; // Nom du capteur 2, ici représenté par une pression

/**
 * @brief Tâche FreeRTOS dédiée à la gestion du capteur 2.
 * 
 * Cette tâche simule la lecture d'une valeur de capteur (exemple : volume) et met à jour la variable 
 * globale `capteur2` avec une valeur fixe. Dans un cas réel, cette fonction pourrait lire la valeur depuis 
 * un capteur connecté à l'ESP32. La tâche est configurée pour se mettre en veille pendant un certain intervalle 
 * de temps (défini par `CAPTEUR2_INTERVAL_MS`) avant de mettre à jour à nouveau la valeur du capteur.
 * 
 * @param pvParameters Paramètre non utilisé, requis par le prototype de la fonction FreeRTOS.
 */
void tache_capteur2(void *pvParameters)
{
    // Boucle infinie de la tâche
    for (;;)
    {
        // Met à jour la valeur du capteur avec une donnée fixe (exemple : pression 
        // Remarque : dans un cas réel, cette ligne serait remplacée par la lecture réelle du capteur.
        capteur2 = random(0, 10001) / 100.0; // Génère un entier de 0 à 10000, divisé par 100.0

        // Temporisation de la tâche avant la prochaine mesure (en millisecondes)
        vTaskDelay(CAPTEUR2_INTERVAL_MS / portTICK_PERIOD_MS);
    }
}
