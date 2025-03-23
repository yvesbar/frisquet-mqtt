#include <Arduino.h>
#include <RadioLib.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <heltec.h>
#include <Preferences.h>

#include "interfaces/Mqtt.h"
#include "modele/Chaudiere.h"
#include "interfaces/VisioConnect.h"

Mqtt * mqtt;
Chaudiere* frisquet;
VisioConnect* visioConnect;

void setup() {
    DEBUG_INIT

    initWifi();

    frisquet = new Chaudiere();

    //TODO ajouter le connecte (si présent)
    //TODO ajouter la sonde externe (si présent)
    //TODO le faire lors de la réception de trames sauf pour les choses émulées -> activées par config.h

    mqtt = new Mqtt();
    mqtt->init();

    visioConnect = new VisioConnect(frisquet);
    visioConnect->init();
}

void initWifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        DEBUGLN(F("INIT - Erreur connexion Wifi - reboot"));
        delay(5000);
        ESP.restart();
    }

    DEBUG(F("INIT - Adresse IP : "));
    DEBUGLN(WiFi.localIP());
}

void loop() {

    //TODO Faire le deploy lors de la découverte des zones/capteurs...
    mqtt->deployAutoDiscoveryHA(frisquet);

    visioConnect->lireTrame();

    mqtt->loop();

}