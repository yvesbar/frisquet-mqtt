#include <Arduino.h>
#include <RadioLib.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <heltec.h>
#include <Preferences.h>

#include "interfaces/Mqtt.h"
#include "modele/Chaudiere.h"

Mqtt * mqtt;
Chaudiere* frisquet;

void setup() {
    DEBUG_INIT

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    frisquet = new Chaudiere();

    //TODO ajouter le connecte (si présent)
    //TODO ajouter la sonde externe (si présent)

    mqtt = new Mqtt();
    mqtt->init();
}

void loop() {
    mqtt->deployAutoDiscoveryHA(frisquet);

    
}