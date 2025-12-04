/**
 * @file tache_luminosite.h
 * @brief Header de la tâche de gestion du capteur de luminosité Grove Light Sensor.
 * 
 * Ce fichier déclare la tâche FreeRTOS qui lit les données du capteur de luminosité
 * et les convertit en lumens pour l'affichage.
 */

#ifndef TACHE_LUMINOSITE
#define TACHE_LUMINOSITE

#include "variablesGlobales.h"

// Variable globale pour stocker la valeur de luminosité en lux (approximatif)
extern volatile float luminosite;

// Nom du capteur
extern const char* Nom_luminosite;

// Tâche FreeRTOS pour la mesure de la luminosité
void tache_luminosite(void *pvParameters);

#endif
