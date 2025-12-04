/**
 * @file tache_oled.cpp
 * @brief Implémentation de la tâche de gestion de l'affichage OLED via le contrôleur SSD1306.
 * 
 * Ce fichier définit une tâche FreeRTOS qui contrôle l'affichage OLED pour afficher les informations des capteurs et l'état du système.
 */

#include "taches/tache_oled.h"
#include "variablesGlobales.h"

/**
 * @brief Initialisation de la bibliothèque SSD1306 et de la tâche d'affichage.
 * 
 * Ce thread utilise la bibliothèque SSD1306Wire pour gérer l'écran OLED en utilisant le protocole I2C.
 * Il permet d'afficher diverses informations, telles que les valeurs des capteurs, la date et l'heure.
 */

// Pour une connexion via I2C en utilisant la bibliothèque Wire d'Arduino.
#include <Wire.h>
#include "SSD1306Wire.h" // Bibliothèque pour gérer le SSD1306
#include "OLEDDisplayUi.h" // Interface utilisateur pour l'affichage OLED
#include "images.h" // Inclusion des images à afficher sur l'écran OLED
#include <WiFi.h> // Bibliothèque pour gérer la connexion WiFi

// Obtenir l'adresse MAC de l'ESP32 pour affichage en mode configuration
String macAddress = WiFi.macAddress();

// Initialisation de l'écran OLED en utilisant l'adresse I2C 0x3c et les broches SDA et SCL.
SSD1306Wire display(0x3c, SDA, SCL);
OLEDDisplayUi ui(&display);

/**
 * @brief Overlay pour afficher l'horodatage NTP.
 * 
 * Cette fonction est utilisée pour afficher l'horodatage sur l'écran OLED.
 * Elle est appelée par la bibliothèque `OLEDDisplayUi`.
 * 
 * @param display Pointeur vers l'objet OLEDDisplay utilisé pour dessiner sur l'écran.
 * @param state Pointeur vers l'état actuel de l'affichage UI.
 */
void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(30, 0, affHorodatageNtp); // Affiche l'horodatage NTP
}

/**
 * @brief Affichage de l'état de la connexion WiFi.
 * 
 * Cette fonction dessine le logo WiFi sur l'écran OLED en fonction de l'état de connexion.
 * 
 * @param display Pointeur vers l'objet OLEDDisplay.
 * @param state Pointeur vers l'état actuel de l'affichage UI.
 * @param x Position X du dessin.
 * @param y Position Y du dessin.
 */
void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->clear();
  // Affichage du logo WiFi en fonction de l'état de connexion
  if (WiFi.isConnected()) {
    display->drawXbm(20, 0, WiFi_Logo_width, WiFi_Logo_height, LogoWIFION);
  } else {
    display->drawXbm(20, 0, WiFi_Logo_width, WiFi_Logo_height, LogoWIFIOFF);
  }
}

/**
 * @brief Slide d'affichage des informations du capteur 1.
 * 
 * Cette fonction affiche le logo et la valeur actuelle du capteur 1 sur l'écran OLED.
 * 
 * @param display Pointeur vers l'objet OLEDDisplay.
 * @param state Pointeur vers l'état actuel de l'affichage UI.
 * @param x Position X du dessin.
 * @param y Position Y du dessin.
 */
void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->clear();
  display->setFont(ArialMT_Plain_16);
  display->drawXbm(0, 0, temp_width, temp_height, reinterpret_cast<const uint8_t*>(Logo_temp)); // Affiche le logo température
  display->setFont(ArialMT_Plain_24);
  display->drawString(60, 10, String(temperature)); // Affiche la température
  display->drawString(75, 40, "°C"); // Affiche l'unité (température en °C)
}

/**
 * @brief Slide d'affichage des informations du capteur d'humidité.
 * 
 * Cette fonction affiche le logo et la valeur actuelle du capteur d'humidité sur l'écran OLED.
 * 
 * @param display Pointeur vers l'objet OLEDDisplay.
 * @param state Pointeur vers l'état actuel de l'affichage UI.
 * @param x Position X du dessin.
 * @param y Position Y du dessin.
 */
void drawFrame3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->clear();
  display->setFont(ArialMT_Plain_16);
  display->drawXbm(0, 8, volume_width, volume_height, reinterpret_cast<const uint8_t*>(Logo_hum)); // Affiche le logo du capteur 2
  display->setFont(ArialMT_Plain_24);
  display->drawString(60, 10, String(humidite)); // Affiche la valeur d'humidité
  display->drawString(60, 40, "%"); // Affiche l'unité (d'humidité en %)
}

/**
 * @brief Slide d'affichage du taux de CO2.
 * 
 * Cette fonction affiche le logo CO2 et la valeur actuelle du capteur SGP30 sur l'écran OLED.
 * 
 * @param display Pointeur vers l'objet OLEDDisplay.
 * @param state Pointeur vers l'état actuel de l'affichage UI.
 * @param x Position X du dessin.
 * @param y Position Y du dessin.
 */
void drawFrame4(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->clear();
  display->setFont(ArialMT_Plain_16);
  display->drawXbm(0, 0, co2_width, co2_height, reinterpret_cast<const uint8_t*>(Logo_co2)); // Affiche le logo CO2
  display->setFont(ArialMT_Plain_24);
  display->drawString(60, 10, String(co2)); // Affiche la valeur de CO2 en ppm
  display->drawString(60, 40, "ppm"); // Affiche l'unité (ppm)
}

/**
 * @brief Slide d'affichage de la date et de l'heure.
 * 
 * Cette fonction affiche la date et l'heure actuelles synchronisées via NTP.
 * 
 * @param display Pointeur vers l'objet OLEDDisplay.
 * @param state Pointeur vers l'état actuel de l'affichage UI.
 * @param x Position X du dessin.
 * @param y Position Y du dessin.
 */
void drawFrame5(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->clear();
  display->setFont(ArialMT_Plain_16);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(0, 0, affHorodatageNTPDate); // Affiche la date NTP
  display->setFont(ArialMT_Plain_24);
  display->drawString(54, 28, affHorodatageNTPHeure); // Affiche l'heure NTP
}

/**
 * @brief Slide d'affichage de la luminosité.
 * 
 * Cette fonction affiche le logo soleil et la valeur actuelle du capteur de luminosité sur l'écran OLED.
 * 
 * @param display Pointeur vers l'objet OLEDDisplay.
 * @param state Pointeur vers l'état actuel de l'affichage UI.
 * @param x Position X du dessin.
 * @param y Position Y du dessin.
 */
void drawFrame6(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->clear();
  display->setFont(ArialMT_Plain_16);
  display->drawXbm(0, 0, lum_width, lum_height, reinterpret_cast<const uint8_t*>(Logo_lum)); // Affiche le logo soleil
  display->setFont(ArialMT_Plain_24);
  display->drawString(60, 10, String((int)luminosite)); // Affiche la valeur de luminosité en lux
  display->drawString(60, 40, "lux"); // Affiche l'unité (lux)
}


// Tableau de pointeurs vers les différentes frames d'affichage
FrameCallback frames[] = { drawFrame1, drawFrame2, drawFrame3, drawFrame4, drawFrame5, drawFrame6 };
int frameCount = 6; // Nombre de frames (ici 6 slides)

// Overlays statiques qui se dessinent par-dessus une frame (exemple : horloge)
OverlayCallback overlays[] = { msOverlay };
int overlaysCount = 1;

/**
 * @brief Affiche une barre de progression sur l'écran OLED.
 * 
 * Cette fonction dessine une barre de progression en bas de l'écran pour indiquer l'état de la mesure.
 * 
 * @param numeroIndex Index de la progression.
 * @param tauxMultiplicateur Multiplicateur de la progression pour ajuster la largeur.
 * @param position Position de la barre de progression sur l'axe Y.
 */
void barreDeProgressionMesure(unsigned int numeroIndex, unsigned int tauxMultiplicateur, unsigned int position) {
  const byte Y = SCREEN_HEIGHT - position;
  display.fillRect(0, Y, numeroIndex * tauxMultiplicateur, 4); // Dessine un rectangle pour la barre de progression
}

/**
 * @brief Tâche FreeRTOS dédiée à la gestion de l'affichage OLED via le SSD1306.
 * 
 * Cette tâche met à jour l'écran OLED avec les informations des capteurs, l'état de la connexion WiFi et l'horodatage.
 * 
 * @param pvParameters Paramètre non utilisé, requis par le prototype de la fonction FreeRTOS.
 */
void tache_oled(void *pvParameters)
{
  // Configuration initiale de l'interface utilisateur OLED
  ui.setTargetFPS(35); // Définit la fréquence d'images (images par seconde) cible à 35 FPS
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);
  ui.setIndicatorPosition(RIGHT); // Position des indicateurs : à droite
  ui.setIndicatorDirection(LEFT_RIGHT); // Direction des transitions : de gauche à droite
  ui.setFrameAnimation(SLIDE_LEFT); // Type de transition : glissement à gauche
  ui.setFrames(frames, frameCount); // Ajoute les frames à l'interface

  // Initialisation de l'interface UI (affiche également l'écran)
  ui.init();

  // Inverse l'écran OLED pour une orientation correcte
  display.flipScreenVertically();
  display.flipScreenVertically();

  // Si l'affichage OLED est désactivé, on l'efface avant de couper la connection (sinon la derniére image ou texte reste affiché)
  if (OLED == false) {
    display.clear();
    display.display();
  }

  // Boucle infinie de la tâche
  for (;;)
  {
    // Si configPortail est activé, affiche les informations de configuration du portail captif
    while (configPortail == true)
    {
      display.setContrast(255);
      display.clear();
      display.setFont(ArialMT_Plain_10);
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.drawString(0, 0, "Portail Captif demandé");
      display.drawString(0, 15, "SSID: ");
      display.drawString(0 + display.getStringWidth("SSID: "), 15, IOTName);
      display.drawString(0, 30, "192.168.4.1");
      display.drawString(0, 45, macAddress);
      display.display();
    }

    // Mise à jour de l'interface UI et gestion du temps restant avant la prochaine mise à jour
    int remainingTimeBudget = ui.update();
    if (remainingTimeBudget > 0) {
      delay(remainingTimeBudget); // Attente pour synchronisation avec la fréquence d'images
    }
  }
}
