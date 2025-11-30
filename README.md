# Smart Campus 2025

Ce projet est une plateforme embarquée basée sur l'ESP32-S2 Saola, développée avec le framework Arduino, pour la gestion intelligente de capteurs et d'actionneurs dans un contexte de Smart Campus.

## Fonctionnalités principales
- **Connexion WiFi** avec portail captif de configuration Web.
- **Affichage OLED** (SSD1306 I2C) pour visualiser les données et l'état du système.
- **Gestion multi-tâches** grâce à FreeRTOS (threads pour chaque fonctionnalité : WiFi, NTP, capteurs, LED RGB, etc.).
- **Synchronisation de l'heure** via NTP.
- **Gestion de LED RGB** (WS2812 intégrée).
- **Support de plusieurs capteurs** (exemples fournis pour deux capteurs, extensible).
- **Paramétrage personnalisé** via le portail web (adresse serveur, identifiants, fuseau horaire, etc.).

## Structure du projet
- `src/` : Code source principal (fichiers .cpp)
- `include/` : Fichiers d'en-tête (.h) et configuration
- `lib/` : Librairies additionnelles
- `test/` : Tests unitaires
- `platformio.ini` : Configuration PlatformIO (environnement ESP32, dépendances, etc.)

## Démarrage rapide
1. Cloner ce dépôt et ouvrir avec PlatformIO (VS Code recommandé).
2. Connecter une carte ESP32-S2 Saola.
3. Compiler et téléverser le firmware.
4. Accéder au portail WiFi pour configurer le réseau et les paramètres personnalisés.

## Dépendances principales
- [ESP8266 and ESP32 OLED driver for SSD1306 displays](https://github.com/ThingPulse/esp8266-oled-ssd1306)
- [NeoPixelBus](https://github.com/Makuna/NeoPixelBus)

## Auteurs
Projet développé par l'équipe Smart Campus 2025, Université de La Réunion.

---
Pour toute question ou contribution, merci de consulter la documentation ou d'ouvrir une issue.
