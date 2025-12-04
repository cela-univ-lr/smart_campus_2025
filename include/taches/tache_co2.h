#ifndef TASK_CO2
#define TASK_CO2
#include "variablesGlobales.h"
#include <Adafruit_SGP30.h>

// Déclaration de l'objet SGP30 (externe)
extern Adafruit_SGP30 sgp30;

// Variable pour stocker la valeur de CO2 (eCO2 en ppm)
extern volatile uint16_t co2;

// Variable pour stocker la valeur de TVOC (ppb) - bonus du SGP30
extern volatile uint16_t tvoc;

// Nom du capteur
extern const char* Nom_co2;

// Fonction d'initialisation du capteur SGP30
bool initSGP30();

// Tâche FreeRTOS pour la mesure du CO2
void tache_co2(void *pvParameters); 

#endif