 /**
 * @file tache_capteur1.cpp
 * @brief Implémentation de la tâche de gestion du capteur 1.
 * 
 * Ce fichier définit une tâche FreeRTOS qui simule la lecture d'un capteur (par exemple, une température).
 * La valeur du capteur est mise à jour à intervalles réguliers et peut être utilisée par d'autres tâches (comme l'affichage OLED).
 */

#include "taches/tache_capteur1.h"
#include "variablesGlobales.h"

// Déclaration de la variable globale pour stocker la valeur du capteur 1 (exemple : température)
volatile float capteur1;

// Nom du capteur 1 (peut être utilisé pour l'affichage ou le débogage)
const char* Nom_capteur1 = "Temp"; 

/**
 * @brief Tâche FreeRTOS dédiée à la gestion du capteur 1.
 * 
 * Cette tâche simule la lecture d'une valeur de capteur (exemple : température) et met à jour la variable 
 * globale `capteur1` avec une valeur fixe. Dans un cas réel, cette fonction pourrait lire la valeur depuis 
 * un capteur connecté à l'ESP32. La tâche est configurée pour se mettre en veille pendant un certain intervalle 
 * de temps (défini par `CAPTEUR1_INTERVAL_MS`) avant de mettre à jour à nouveau la valeur du capteur.
 * 
 * @param pvParameters Paramètre non utilisé, requis par le prototype de la fonction FreeRTOS.
 */
void tache_capteur1(void *pvParameters)
{ 
    // Délai initial pour s'assurer que les initialisations du système sont terminées
    delay(100); 

    // Boucle infinie de la tâche
    for (;;)
    {
        // Met à jour la valeur du capteur avec une donnée fixe (exemple : température )
        // Remarque : dans un cas réel, cette ligne serait remplacée par la lecture réelle du capteur.
        capteur1 = random(0, 10001) / 100.0; // Génère un entier de 0 à 10000, divisé par 100.0


        // Temporisation de la tâche avant la prochaine mesure (en millisecondes)
        vTaskDelay(CAPTEUR1_INTERVAL_MS / portTICK_PERIOD_MS);
    }
}
