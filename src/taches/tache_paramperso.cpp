/**
 * @file tache_paramperso.cpp
 * @brief Implémentation de la tâche de gestion des paramètres personnalisés.
 * 
 * Ce fichier définit une tâche FreeRTOS qui affiche les paramètres personnalisés saisis sur le portail Web de l'ESP32.
 * Les paramètres sont affichés sur le terminal série à des intervalles réguliers pour visualiser les données saisies.
 */

#include "taches/tache_paramperso.h"
#include "variablesGlobales.h"
#include <WiFiManager.h> // Bibliothèque pour gérer les connexions WiFi et les paramètres via un portail captif

/**
 * @brief Tâche FreeRTOS pour afficher les paramètres personnalisés sur le terminal série.
 * 
 * Cette tâche s'assure que l'ESP32 est connecté au WiFi avant d'afficher les paramètres personnalisés 
 * définis par l'utilisateur sur le portail Web. Les informations sont affichées sur le terminal série 
 * toutes les 10 secondes.
 * 
 * @param parameter Paramètre non utilisé, requis par le prototype de la fonction FreeRTOS.
 */
void parametres_perso(void * parameter) {
    // Attendre que l'ESP32 soit connecté au WiFi avant de poursuivre
    while (WiFi.status() != WL_CONNECTED) {    
        // Attente de 100ms avant de vérifier à nouveau la connexion WiFi
        delay(100);
    }

    // Boucle infinie de la tâche
    for(;;) {
        // Affiche les différents paramètres personnalisés saisis par l'utilisateur
        Serial.println("[MES PARAMS] Mon serveur : " + String(params.adr_mon_serveur)); // Adresse du serveur
        Serial.println("[MES PARAMS] Mon Port: " + String(params.port_mon_serveur));    // Port du serveur
        Serial.println("[MES PARAMS] Nom login: " + String(params.user_mon_serveur));   // Nom d'utilisateur pour le serveur
        Serial.println("[MES PARAMS] Mot de passe : " + String(params.pass_mon_serveur)); // Mot de passe pour le serveur
        Serial.println("[MES PARAMS] Données: " + String(params.data_mon_serveur));     // Données supplémentaires saisies

        // Attendre 10 secondes avant d'afficher à nouveau les paramètres
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}
