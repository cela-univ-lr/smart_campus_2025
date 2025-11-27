/**
 * @file tache_ledrgb.cpp
 * @brief Implémentation de la tâche de gestion de la LED RGB avec la bibliothèque NeoPixelBus.
 * 
 * Ce fichier définit une tâche FreeRTOS qui contrôle une LED RGB en fonction de l'état d'une variable.
 */

#include <Arduino.h>
#include "taches/tache_ledrgb.h"
#include "variablesGlobales.h"

// Déclaration d'un objet strip pour gérer les LEDs NeoPixel avec un ordre de couleur NeoGrb et une méthode de contrôle WS2812x.
NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);
// Déclaration alternative d'un objet strip avec un ordre de couleur NeoRgb et une méthode de contrôle 400Kbps (décommenter pour utiliser).
// NeoPixelBus<NeoRgbFeature, Neo400KbpsMethod> strip(PixelCount, PixelPin);

/**
 * @brief Tâche FreeRTOS pour contrôler la LED RGB.
 * 
 * Cette tâche gère les effets lumineux de la LED RGB en fonction de l'état de la variable globale `configPortail`.
 * 
 * @param pvParameters Paramètre non utilisé, requis par le prototype de la fonction FreeRTOS.
 */
void task_ledrgb( void *pvParameters )
{
    // Initialise la bande de LED NeoPixel et éteint toutes les LEDs au démarrage.
    strip.Begin();
    strip.Show();

    // Boucle infinie de la tâche
    for (;;)
    {
        // Si configPortail est égal à 1, cela signifie que le capteur est en phase de préchauffe.
        // La LED RGB clignote en blanc avec une fréquence de 1Hz (500ms allumée, 500ms éteinte).
        while (configPortail == 1)
        {
            // Allume la LED en blanc avec une luminosité de 20% (R=20, G=20, B=20).
            strip.SetPixelColor(0, RgbColor(20, 20, 20)); // blanc à 20% de luminosité
            strip.Show();  // Met à jour la LED
            vTaskDelay(500); // Attente de 500ms

            // Éteint la LED.
            strip.SetPixelColor(0, RgbColor(0, 0, 0)); // OFF
            strip.Show();  // Met à jour la LED
            vTaskDelay(500); // Attente de 500ms
        }

        // Si configPortail n'est pas en préchauffe, on s'assure que la LED est éteinte.
        strip.SetPixelColor(0, RgbColor(0, 0, 0)); // OFF
        strip.Show(); // Met à jour la LED

        // Code supplémentaire d'exemple pour contrôler la LED avec différentes couleurs (commenté).
        /*
        strip.SetPixelColor(0, RgbColor(255, 0, 0));      // rouge
        strip.SetPixelColor(0, RgbColor(0, 127, 0));      // vert
        strip.SetPixelColor(0, RgbColor(0, 0, 53));       // bleu
        strip.SetPixelColor(0, RgbwColor(0, 0, 128, 255)); // canal blanc avec un peu de bleu
        strip.SetPixelColor(0, RgbColor(0, 20, 0));       // vert à 20% de luminosité
        strip.Show(); // Met à jour la LED
        vTaskDelay(500); // Attente de 500ms
        strip.SetPixelColor(0, RgbColor(0, 0, 0)); // OFF
        strip.Show(); // Met à jour la LED
        vTaskDelay(500); // Attente de 500ms
        */
    }
}
