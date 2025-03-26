#ifndef __MODULE_HELTEC_H_
#define __MODULE_HELTEC_H_

#include <Arduino.h>
#include <WiFi.h>
#include <heltec.h>
#include <RadioLib.h>
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

    static SX1262 radio;

private:
};

#endif // __MODULE_HELTEC_H_
