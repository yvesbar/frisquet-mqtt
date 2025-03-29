#ifndef __MODULE_HELTEC_H_
#define __MODULE_HELTEC_H_

#include <Arduino.h>
#include <WiFi.h>
#include <heltec.h>
#include <RadioLib.h>
#include <Preferences.h>
#include "image.h"
#include "../debug.h"
#include "../config.h"

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

    //gestion de la mémoire flash du module
    //TODO à gérer mais vu que c'est déjà dans le config.h, je ne sais pas si c'est utile
    //static void initNvs();
    //static void eraseNvs();

    static SX1262 radio;

private:
    static Preferences preferences; // Référence statique pour les préférences
};

#endif // __MODULE_HELTEC_H_
