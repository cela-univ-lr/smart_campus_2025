#ifndef CONFIGURATION
#define CONFIGURATION

/**
 * Nom et password de l'accés au portail captif de l'ESP32
 */
#define NOM_PROJET "monBeauProjet"
#define PwdPortail "12345678"       
/**
 * Selection des périphériques d'entrées/sorties
 */

// Définition des broches
#define CONFIG_PIN                  0  // broche boot ; si appui alors niveau 0
#define TEMPHUM_PIN                38  // Broche DHT22 (capteur température/humidité)
#define DHTTYPE                  DHT22  // Type de capteur DHT
#define TEMPHUM_INTERVAL_MS      4000  // Intervale de temps de la boucle infinie du capteur temp/hum en ms 
#define CAPTEUR2_INTERVAL_MS      4000  // Intervale de temps de la boucle infinie du capteur1 en ms 

/**
 * TIME NTP
 */

#define NTP_SERVER "fr.pool.ntp.org"

#define NTP_UPDATE_INTERVAL_MS      6000  // en ms

/**
 * WIFI 
 */



/**
 * Dimensions de l'écran OLED
 */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define oled_UPDATE_INTERVAL_MS  3000 // 3 secondes


#endif
