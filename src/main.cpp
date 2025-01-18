#include <Arduino.h>
#include <RadioLib.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

#include "interfaces/Mqtt.h" 

Mqtt * mqtt;

void setup() {
    DEBUG_INIT

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }

    mqtt = new Mqtt();
    mqtt->init();
}

void loop() {
    mqtt->deployConfHA();




}