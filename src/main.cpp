/* Gestionnaire de connexion WiFi pour ESP32 avec portail de configuration Web
- Affichage des données sur écran OLED type I2c SSD1306 grove
- AVEC Threads : FreeRtos
Framework utilisé : Arduino 
Board : ESP32s2 Saola 
*/
#include "variablesGlobales.h"


// Déclaration d'une handle pour la tâche de gestion de l'afficheur OLED
TaskHandle_t tache_oledhandle = NULL;

void setup() {

  Serial.begin(115200); // configuration liason série pour debug en mode Terminal
   xTaskCreate(&tache_oled, "affichage_OLED", 8192*2, NULL, 4, &tache_oledhandle); // Tâche  OLED SSD1306
  
  if (OLED == false){ 

  vTaskDelete(tache_oledhandle);

  }
// Délai pour laisser la tâche OLED tourner un moment avant de lancer les autres tâches : 5 secondes
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  xTaskCreate(&wifi, "WIFI", 8192*2, NULL, 1, NULL); // Thread connecion WIFI

  // Délai pour laisser la tâche WIFI tourner un moment avant de lancer les autres tâches : 5 secondes
  vTaskDelay(5000 / portTICK_PERIOD_MS);

  xTaskCreate(&parametres_perso, "paramtres perso", 8192, NULL, 2, NULL); // Tâche de traitement des paramétres entrés dans le portail wifi section "Configuration"

  xTaskCreate(&fetchTimeFromNTP,"Update_NTP_time", 8192,NULL, 3,NULL); // TTâche  Time NTP 

  xTaskCreate(&task_ledrgb, "LEDRGB", 8192, NULL, 5, NULL); // Tâche LED RGB WS2812 interne 
 
  xTaskCreate(&tache_tempHum,"TempHum DHT22", 8192, NULL, 6, NULL); // Thread température/humidité DHT22

  xTaskCreate(&tache_capteur2,"capteur capteur2", 8192, NULL, 7, NULL);

  // Ajouter d'autres tâches pour d'autres capteurs.....

  Serial.println("\n Démarrage");

}



void loop() {

 
}

