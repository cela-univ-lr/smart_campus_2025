/**
 * @file tache_Ntp.cpp
 * @brief Implémentation de la tâche de gestion du temps via le protocole NTP.
 * 
 * Ce fichier définit une tâche FreeRTOS qui récupère et met à jour l'heure locale depuis un serveur NTP.
 * L'heure est ensuite affichée et peut être utilisée pour d'autres opérations (affichage sur OLED, envoi de données via MQTT, etc.).
 */

#include "taches/tache_Ntp.h"
#include "time.h"
#include "variablesGlobales.h"
#include <WiFiManager.h>

// Drapeau pour indiquer l'envoi de données (variable volatile car modifiée par des interruptions ou des tâches)
volatile bool Flag_Envoi_donnees = false;

// Déclaration des paramètres personnalisés
extern CustomParams params;

// Variables pour l'affichage de l'horodatage et de l'heure locale
char affHorodatageNtp[50]; 
char horodatageNtp[15]; 
char affHorodatageNTPDate[10];
char affHorodatageNTPHeure[6];

// Fuseau horaire par défaut (GMT+2)
long tzf = 2;

// Jours de la semaine en français
String weekDays[7] = {"Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi"};

// Mois de l'année en français
String months[12] = {"Janvier", "Février", "Mars", "Avril", "Mai", "Juin", "Juillet", "Août", "Septembre", "Octobre", "Novembre", "Décembre"};

// Dernière heure d'envoi de données (en millisecondes)
unsigned long lastSendTime = 0;

// Intervalle de temps pour l'envoi de données (15 minutes en millisecondes)
const unsigned long interval = 15 * 60 * 1000;

// Nom du serveur NTP à interroger
const char* ntpServer = NTP_SERVER;

/**
 * @brief Affiche l'heure locale récupérée depuis le serveur NTP.
 * 
 * Cette fonction récupère l'heure locale en utilisant le fuseau horaire configuré par l'utilisateur.
 * Si l'obtention de l'heure échoue, un message d'erreur est affiché. Les horodatages sont stockés 
 * dans des variables globales pour être utilisés par d'autres tâches (OLED, MQTT, etc.).
 */
void printLocalTime(){
    struct tm timeinfo;

    // Vérifie si le fuseau horaire est défini dans les paramètres personnalisés
    if (String(params.timeZone) != NULL) {
        tzf = String(params.timeZone).toInt(); // Récupère le fuseau horaire configuré
    } else {
        tzf = 2; // Fuseau horaire par défaut (GMT+2)
    }

    // Configuration du fuseau horaire et des offsets pour le calcul de l'heure locale
    const long gmtOffset_sec = 3600 * tzf;
    const int daylightOffset_sec = 3600 * 0;

    // Configuration du fuseau horaire avec le serveur NTP
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    // Récupère l'heure locale depuis le serveur NTP
    if (!getLocalTime(&timeinfo)) {
        Serial.println("[NTP] Échec de l'obtention de l'heure"); // Affiche un message d'erreur
        return;
    }

    // Affiche l'heure complète en format lisible pour le débogage
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

    // Formate les différentes parties de l'horodatage pour affichage OLED et envoi MQTT
    strftime(affHorodatageNTPDate, 10, "%D", &timeinfo);  // Date (format mm/dd/yy) pour OLED
    strftime(affHorodatageNTPHeure, 6, "%H:%M", &timeinfo);  // Heure (format HH:MM) pour OLED
    strftime(affHorodatageNtp, 50, "%D %H:%M", &timeinfo);  // Horodatage complet pour OLED
    strftime(horodatageNtp, 15, "%D %H:%M", &timeinfo);  // Horodatage complet pour MQTT
}

/**
 * @brief Tâche FreeRTOS pour récupérer l'heure depuis le serveur NTP.
 * 
 * Cette tâche se connecte au serveur NTP pour récupérer l'heure et met à jour régulièrement l'horodatage.
 * L'heure est mise à jour à chaque intervalle défini (toutes les 15 minutes par défaut) et le drapeau 
 * `Flag_Envoi_donnees` est activé pour signaler qu'un envoi de données est prêt.
 * 
 * @param parameter Paramètre non utilisé, requis par le prototype de la fonction FreeRTOS.
 */
void fetchTimeFromNTP(void * parameter){
    // Attente jusqu'à ce que le module WiFi soit connecté
    while (WiFi.status() != WL_CONNECTED) {
        delay(100); // Attendre 100 ms avant de vérifier à nouveau
    }

    // Récupère l'heure locale à partir du serveur NTP
    printLocalTime();

    // Boucle infinie de la tâche
    for (;;)
    {
        // Obtenir l'heure actuelle en millisecondes
        unsigned long currentTime = millis();

        // Vérifier si l'intervalle de 15 minutes est écoulé depuis le dernier envoi
        if (currentTime - lastSendTime >= interval) {
            lastSendTime = currentTime; // Met à jour le dernier temps d'envoi
            Flag_Envoi_donnees = true;  // Active le drapeau d'envoi de données
        }

        // Temporisation de la tâche pour éviter les appels excessifs
        vTaskDelay(NTP_UPDATE_INTERVAL_MS / portTICK_PERIOD_MS);
    }
}
