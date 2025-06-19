#ifndef __MODULE_HELTEC_H_
#define __MODULE_HELTEC_H_

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <heltec.h>
#include <RadioLib.h>
#include <Preferences.h>
#include "image.h"
#include "../debug.h"
#include "../config.h"
#include "../modele/Connect.h"

class ModuleHeltec {
public:
    // Méthode statique pour initialiser le WiFi
    static void initWifi(const char* ssid, const char* password);

    // Méthode statique pour initialiser le module Heltec
    static void initRadio();

    // Méthode statique pour initialiser l'affichage
    static void initAffichage();

    // Affiche le message indiqué sur l'écran OLED
    static void affiche(const String& message);

    // Méthode statique pour initialiser l'OTA
    static void initOTA();
    static void handleOTA();

    //gestion de la mémoire flash du module
    static void initNVS();

    static void saveIdConnect(byte id);
    static byte loadIdConnect();

    static SX1262 radio;

private:
    static Preferences preferences; // Référence statique pour les préférences
};

#endif // __MODULE_HELTEC_H_
