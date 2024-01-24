#include "Mqtt.h"
#include "config.h"

Mqtt::Mqtt() {
    WiFiClient* wifi = new WiFiClient();
    client = new PubSubClient(*wifi);
}

void Mqtt::init() {
    client->setServer(mqttServer, mqttPort);
    client->setBufferSize(2048);

    while (!client->connected()) {
        Serial.println("Mqtt - Connexion");
        
        if (client->connect("ESP32 Frisquet", mqttUsername, mqttPassword)) {
            Serial.println("Mqtt - Connecté");
        }
        else {
            Serial.print("Mqtt - Erreur de connexion, rc=");
            Serial.print(client->state());
            Serial.println(" attente 5 sec ...");
            delay(5000);
        }
    }
}

