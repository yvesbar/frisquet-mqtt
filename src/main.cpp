#include <Arduino.h>
#include <RadioLib.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <heltec.h>
#include <Preferences.h>

#include "interfaces/MqttPub.h"
#include "interfaces/MqttSub.h"
#include "modele/Chaudiere.h"
#include "interfaces/VisioConnectSub.h"


MqttPub * mqttPub;
MqttSub * mqttSub;
Chaudiere* chaudiere;
VisioConnectSub* visioConnectSub;

void setup() {
    DEBUG_INIT

    initWifi();

    chaudiere = new Chaudiere();

    //TODO ajouter le connecte (si présent)
    //TODO ajouter la sonde externe (si présent)
    //TODO le faire lors de la réception de trames sauf pour les choses émulées -> activées par config.h

    //Création des connexions mqtt dans les deux sens (Publish/Subscribe)
    mqttPub = new MqttPub();
    mqttPub->init();
    mqttSub = new MqttSub(mqttPub->getClient());
    mqttSub->init();

    visioConnectSub = new VisioConnectSub(chaudiere, mqttPub);
    visioConnectSub->init();

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
    mqttPub->deployAutoDiscoveryHA(chaudiere);

    visioConnectSub->lireTrame();

    mqttPub->loop();
    mqttSub->loop();
}