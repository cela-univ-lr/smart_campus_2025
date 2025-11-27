#include "WiFiManager.h" // Bibliothèque pour gérer les connexions WiFi
#include "taches/tache_Wifi.h" // Tâches personnalisées pour WiFi
#include <Arduino.h> // Bibliothèque principale Arduino
#include "variablesGlobales.h" // Variables globales du projet
#include "SPIFFS.h" // Système de fichiers pour ESP32
#include <FS.h> // Interface pour le système de fichiers


// Variables globales pour la configuration

const char* modes[] = { "NULL", "STA", "AP", "STA+AP" }; // Modes WiFi disponibles

volatile bool configPortail = false; // Indicateur pour le portail de configuration
const char IOTName[] = NOM_PROJET; // Nom du projet IoT
const char pwdportail[] = PwdPortail; // Mot de passe pour le portail WiFi

// Options de test
bool TEST_CP = false; // Forcer l'ouverture du portail de configuration même si le WiFi est trouvé
int  TESP_CP_TIMEOUT = 90; // Timeout pour le portail de configuration
bool TEST_NET = true; // Tester le réseau après connexion (ex. récupérer l'heure NTP)
bool ALLOWONDEMAND = true; // Activer la demande de portail via un bouton
int  ONDDEMANDPIN = CONFIG_PIN; // GPIO pour le bouton
bool WMISBLOCKING = true; // Mode bloquant pour WiFiManager
uint8_t BUTTONFUNC = 1; // Fonction du bouton : 0 pour réinitialiser, 1 pour config, 2 pour autoConnect
char ssid[] = "******"; // Nom du réseau WiFi par défaut
char pass[] = "*****"; // Mot de passe du réseau WiFi par défaut

bool OLED = 1 ; // Activation ou non de l'écran OLED par defaut : Activé

char _oled_checkbox[34] = "type=\"checkbox\""; // pour les checkbox

// Structure pour stocker les paramètres MQTT
CustomParams params = {
  "https://hostname.fr", "8080", "user", "password", "à définir ou pas", "+2"
};

// Instance de WiFiManager
WiFiManager wm;

// Paramètres personnalisés pour WiFiManager
WiFiManagerParameter custom_html("<p style=\"color:pink;font-weight:Bold;\">Page de Configuration</p>");
WiFiManagerParameter custom_server("server", "Adresse de mon serveur", params.adr_mon_serveur, 80);
WiFiManagerParameter custom_port("port", "Numéro de port", params.port_mon_serveur, 6, "pattern='\\d{1,6}' title='5 chiffres maximum'");
WiFiManagerParameter custom_user("user", "Nom de login", params.user_mon_serveur, 32);
WiFiManagerParameter custom_pass("pass", "Mot de passe", params.pass_mon_serveur, 32);
WiFiManagerParameter custom_data("data", "Donneés particuliéres", params.data_mon_serveur, 32);
WiFiManagerParameter custom_GMT("gmt", "GMT exemple:+2", params.timeZone, 3, "pattern='^[\\+\\-]\\d{1}$' title='Seulement un signe (+ ou -) et un chiffre'");
WiFiManagerParameter custom_checkbox_oled("oled_en", "OLED Activé", "true", 4, _oled_checkbox, WFM_LABEL_AFTER);

// Fonction pour afficher les informations sur la connexion WiFi Mqtt ....
void Info() {
  Serial.println("");
  Serial.println("[WIFI] DEBUG DES INFORMATIONS WIFI");
  Serial.println("[WIFI] SAUVEGARDE: " + (String)(wm.getWiFiIsSaved() ? "OUI" : "NON"));
  Serial.println("[WIFI] SSID: " + (String)wm.getWiFiSSID());
  Serial.println("[WIFI] PASS: " + (String)wm.getWiFiPass());
  Serial.println("[WIFI] NOM D'HÔTE: " + (String)WiFi.getHostname());
  Serial.println("[MES PARAMS] Mon serveur : " + String(params.adr_mon_serveur));
  Serial.println("[MES PARAMS] Mon  Port: " + String(params.port_mon_serveur));
  Serial.println("[MES PARAMS] Nom login: " + String(params.user_mon_serveur));
  Serial.println("[MES PARAMS] Mot de passe : " + String(params.pass_mon_serveur));
  Serial.println("[MES PARAMS] Données: " + String(params.data_mon_serveur));
  Serial.println("[NTP]Time Zone: " + String(params.timeZone));
  Serial.println(OLED ? "[OLED] activé" : "[OLED] désactivé");
}



// Fonction pour sauvegarder les paramètres personnalisés dans SPIFFS
void saveCustomParameters() {
 

  // Récupérer les valeurs du formulaire HTML
  strcpy(params.adr_mon_serveur, custom_server.getValue());
  strcpy(params.port_mon_serveur, custom_port.getValue());
  strcpy(params.user_mon_serveur, custom_user.getValue());
  strcpy(params.pass_mon_serveur, custom_pass.getValue());
  strcpy(params.data_mon_serveur, custom_data.getValue());
  strcpy(params.timeZone, custom_GMT.getValue());
   // Sauvegarder la valeur de la checkbox OLED
    const char* oled_value = custom_checkbox_oled.getValue();
    Serial.print("[SAVE]oled_value : ");
    Serial.println(oled_value);
        OLED = (oled_value != nullptr && strcmp(oled_value, "true") == 0); // Si la valeur est non vide, OLED est activé
    Serial.print("[SAVE]OLED: ");
    Serial.println(OLED);
  // Sauvegarder les paramètres dans SPIFFS
  File configFile = SPIFFS.open("/config.txt", "w");
  if (configFile) {
    String data = String(params.adr_mon_serveur) + "," + String(params.port_mon_serveur) + "," +
                  String(params.user_mon_serveur) + "," + String(params.pass_mon_serveur) + "," +
                  String(params.data_mon_serveur) + "," + String(params.timeZone) + "," +
                  (OLED ? "1" : "0");
    configFile.println(data);
    configFile.close();
    Serial.println("Paramètres sauvegardés dans SPIFFS.");
  }
}

void restoreCustomParameters() {
    if (SPIFFS.exists("/config.txt")) {
        File configFile = SPIFFS.open("/config.txt", "r");
        if (configFile) {
            String content = configFile.readString();
            configFile.close();

            // Imprimer le contenu du fichier pour déboguer
            Serial.print("[RESTORE] Contenu de config.txt: ");
            Serial.println(content);

            int lastComma = -1;
            for (int i = 0; i < 7; i++) {
                int nextComma = content.indexOf(',', lastComma + 1);
                if (nextComma == -1) nextComma = content.length();
                String param = content.substring(lastComma + 1, nextComma);
                Serial.print("[RESTORE] Paramètre ");
                Serial.print(i);
                Serial.print(": ");
                Serial.println(param);

                switch (i) {
                    case 0: param.toCharArray(params.adr_mon_serveur, 80); break;
                    case 1: param.toCharArray(params.port_mon_serveur, 6); break;
                    case 2: param.toCharArray(params.user_mon_serveur, 32); break;
                    case 3: param.toCharArray(params.pass_mon_serveur, 32); break;
                    case 4: param.toCharArray(params.data_mon_serveur, 32); break;
                    case 5: param.toCharArray(params.timeZone, 3); break;
                    case 6: OLED = (param.toInt() == 1); // Si le paramètre est "1", OLED = true

                        Serial.print("[RESTORE]OLED: ");
                        Serial.println(OLED ? "1" : "0");

                        break;
                }
                lastComma = nextComma;
            }
        }
    }
            // Initialiser les paramètres dans WiFiManager
              custom_GMT.setValue(params.timeZone, 3);
              custom_server.setValue(params.adr_mon_serveur, 80);
              custom_port.setValue(params.port_mon_serveur, 6);
              custom_user.setValue(params.user_mon_serveur, 32);
              custom_pass.setValue(params.pass_mon_serveur, 32);
              custom_data.setValue(params.data_mon_serveur, 32);
            // Charger la valeur de OLED à partir du fichier SPIFFS (dernier élément)

              if (OLED) {
                
                Serial.print("[RESTORE]OLED = 1");
                snprintf(_oled_checkbox, sizeof(_oled_checkbox), "type=\"checkbox\" checked");

              } else {
                Serial.print("[RESTORE]OLED = 0");
                snprintf(_oled_checkbox, sizeof(_oled_checkbox), "type=\"checkbox\"");
              }

}

// CALLBACK

// Callback déclenché lorsque les paramètres WiFi sont sauvegardés
void saveWifiCallback() {
  Serial.println("[CALLBACK] Sauvegarde des paramètres WiFi");
}

// Callback déclenché lorsque WiFiManager entre en mode configuration
void configModeCallback(WiFiManager *myWiFiManager) {
  Serial.println("[CALLBACK] Mode configuration activé");
}
// Callback pour sauvegarder les paramètres lorsque l'utilisateur valide
void saveParamCallback() {
  Serial.println("[CALLBACK] Sauvegarde des paramètres personnalisés");
  saveCustomParameters();
}

// Fonction pour gérer les requêtes HTTP personnalisées
void handleRouteEraseSpiffs() {

    SPIFFS.begin();
    SPIFFS.format();
    wm.server->send(200, "text/plain", "SPIFFS effacé !");
  
}

// Attache des routes supplémentaires pour le serveur
void bindServerCallback() {
  wm.server->on("/erasespiffs", handleRouteEraseSpiffs); // Route personnalisée
}

// Callback pour gérer les mises à jour OTA
void handlePreOtaUpdateCallback() {
  Update.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progression OTA : %u%%\r", (progress / (total / 100)));
  });
}



// Fonction principale pour gérer la configuration WiFi et les paramètres
void wifi(void *pvParameters) {
  

  // Initialisation de SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Échec de l'initialisation de SPIFFS");
    return;
  }

  restoreCustomParameters(); // Restaurer les paramètres depuis SPIFFS

  WiFi.setSleep(true); // Activer le mode veille pour économiser l'énergie
  wm.setDebugOutput(false); // Désactiver les sorties de débogage WiFiManager

  // Configuration de WiFiManager
  wm.addParameter(&custom_html);
  wm.addParameter(&custom_server);
  wm.addParameter(&custom_port);
  wm.addParameter(&custom_user);
  wm.addParameter(&custom_pass);
  wm.addParameter(&custom_data);
  wm.addParameter(&custom_GMT);
  wm.addParameter(&custom_checkbox_oled);

  // Définir les callbacks pour différents événements
  wm.setAPCallback(configModeCallback);
  wm.setWebServerCallback(bindServerCallback);
  wm.setSaveConfigCallback(saveWifiCallback);
  wm.setSaveParamsCallback(saveParamCallback);
  wm.setPreOtaUpdateCallback(handlePreOtaUpdateCallback);

  wm.setDarkMode(true); // Activer le mode sombre pour l'interface
  wm.setScanDispPerc(true);


// set custom html menu content , inside menu item "custom", see setMenu()
 const char* menuhtml = 
    "<form action='/erasespiffs' method='get'><button class='D'>Effacer Spiffs</button></form><br/>";
  wm.setCustomMenuHTML(menuhtml);
  

  // Définir un menu personnalisé
  std::vector<const char *> menu = {"wifi", "wifinoscan", "info", "param", "sep", "erase", "custom",  "update", "restart", "exit"};
  wm.setMenu(menu); // Appliquer le menu personnalisé

  wm.setHostname(IOTName); // Définir le nom d'hôte pour l'ESP32

  if (!WMISBLOCKING) {
    wm.setConfigPortalBlocking(false); // Mode non-bloquant
  }

  wm.setConfigPortalTimeout(120); // Timeout du portail de configuration

  //wifiInfo(); // Afficher les informations WiFi

  if (!wm.autoConnect(IOTName, pwdportail)) {
    Serial.println("Échec de la connexion au WiFi et timeout atteint");
  } else if (TEST_CP) {
    vTaskDelay(1000);
    wm.startConfigPortal(IOTName, pwdportail); // Lancer le portail de configuration
  } else {
    Serial.println("Connecté au WiFi");
  }

  Info(); // Réafficher les infos après connexion

  pinMode(ONDDEMANDPIN, INPUT_PULLUP); // Configurer le bouton en entrée avec pull-up

  for (;;) { // Boucle infinie pour gérer la configuration et le WiFi
    if (!WMISBLOCKING) {
      wm.process(); // Gérer les événements non-bloquants de WiFiManager
    }

    // Gestion de la demande de portail via un bouton
    if (ALLOWONDEMAND && digitalRead(ONDDEMANDPIN) == LOW) {
      delay(100); // Anti-rebond
      if (digitalRead(ONDDEMANDPIN) == LOW || BUTTONFUNC == 2) {
        Serial.println("BOUTON APPUYÉ");
        configPortail = true;
       

        if (BUTTONFUNC == 0) {
          wm.resetSettings(); // Réinitialiser les paramètres WiFi
          wm.reboot(); // Redémarrer l'ESP32
          vTaskDelay(200);
          return;
        }

        if (BUTTONFUNC == 1) {
          if (!wm.startConfigPortal(IOTName, pwdportail)) {
            Serial.println("Échec du portail de configuration");
            delay(3000);
          }
          return;
        }

        if (BUTTONFUNC == 2) {
          wm.setConfigPortalTimeout(TESP_CP_TIMEOUT);
          wm.autoConnect(); // Se reconnecter automatiquement
          return;
        }
      } else {
        Serial.println("Connecté au WiFi");
        configPortail = false;
      }
    }
    vTaskDelay(100); // Attendre avant de vérifier à nouveau
  }
}
