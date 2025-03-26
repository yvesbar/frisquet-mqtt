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
#include "interfaces/ModuleHeltec.h"


MqttPub * mqttPub;
MqttSub * mqttSub;
Chaudiere* chaudiere;
VisioConnectSub* visioConnectSub;

void initOTA()
{
    ArduinoOTA.setHostname("ESP32Frisquet");
    ArduinoOTA.setTimeout(25); // Augmenter le délai d'attente à 25 secondes
    ArduinoOTA
        .onStart([]() {
                    String type;
                    if (ArduinoOTA.getCommand() == U_FLASH)
                    type = "sketch";
                    else // U_SPIFFS
                    type = "filesystem";
                    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
                    DEBUGLN("Start updating " + type); })
        .onEnd([]() { DEBUGLN(F("\nEnd")); })
        .onProgress([](unsigned int progress, unsigned int total)
                    { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
        .onError([](ota_error_t error)  {
                    Serial.printf("Error[%u]: ", error);
                    if (error == OTA_AUTH_ERROR) DEBUGLN(F("Auth Failed"));
                    else if (error == OTA_BEGIN_ERROR) DEBUGLN(F("Begin Failed"));
                    else if (error == OTA_CONNECT_ERROR) DEBUGLN(F("Connect Failed"));
                    else if (error == OTA_RECEIVE_ERROR) DEBUGLN(F("Receive Failed"));
                    else if (error == OTA_END_ERROR) DEBUGLN(F("End Failed"));
                });
    ArduinoOTA.begin();
}

void setup() {
    DEBUG_INIT

    ModuleHeltec::initWifi(ssid, password);
    ModuleHeltec::initRadio();
    ModuleHeltec::initAffichage();
    initOTA();

    chaudiere = new Chaudiere();

    // Création des connexions MQTT
    mqttPub = new MqttPub();
    mqttPub->init();
    mqttSub = new MqttSub(mqttPub->getClient());
    mqttSub->init();

    // Initialisation de VisioConnectSub
    visioConnectSub = new VisioConnectSub(chaudiere, mqttPub);
    visioConnectSub->init();
}

void loop() {

    //TODO Faire le deploy lors de la découverte des zones/capteurs...
    mqttPub->deployAutoDiscoveryHA(chaudiere);

    visioConnectSub->lireTrame();

    mqttPub->loop();
    mqttSub->loop();
}