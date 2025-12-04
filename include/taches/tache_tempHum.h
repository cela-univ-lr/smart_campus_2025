#ifndef TACHE_TEMPHUM
#define TACHE_TEMPHUM

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// Sémaphore pour signaler que les premières mesures de température/humidité sont disponibles
// Ce sémaphore est utilisé par la tâche CO2 pour attendre avant de démarrer
extern SemaphoreHandle_t xSemaphoreTempHumReady;

void tache_tempHum(void *pvParameters);

#endif
