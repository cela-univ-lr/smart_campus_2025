#ifndef VARIABLES
#define VARIABLES
#include <Arduino.h>
#include "configuration.h"
#include <Preferences.h>
#include "taches/tache_Ntp.h"
#include "taches/tache_Wifi.h"
#include "taches/tache_oled.h"
#include "taches/tache_paramperso.h"
#include "taches/tache_ledrgb.h"
#include "taches/tache_tempHum.h"
#include "taches/tache_capteur2.h"
#include <WiFiUdp.h>

//ESP32S2
extern String macAddress;


//variable pour le nom de l'IOT et de son SSID
 
extern const char IOTName[];
extern const char pwdportail[];

//variable pour le portail de configuration
extern volatile bool configPortail;

//Variables pour parametres customs html

// Structure pour stocker les paramètres avec des noms spécifiques
struct CustomParams {
  char adr_mon_serveur[80];
  char port_mon_serveur[6];
  char user_mon_serveur[32];
  char pass_mon_serveur[32];
  char data_mon_serveur[32];
  char timeZone[3];
                    };

extern CustomParams params ;

//Pour l'heure par protocole NTP
extern char affHorodatageNtp[50]; 
extern char horodatageNtp[15]; 
extern char affHorodatageNTPDate[10];
extern char affHorodatageNTPHeure[6];

//pour les entrées/sorties
extern bool OLED;
extern const char* Nom_temperature;
extern const char* Nom_humidite;
extern const char* Nom_capteur2;

// pour les capteurs
extern volatile float temperature;
extern volatile float humidite;
extern volatile float capteur2; 

#endif