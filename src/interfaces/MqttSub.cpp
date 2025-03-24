#include "MqttSub.h"

MqttSub::MqttSub(PubSubClient* client) : client(client) {}

void MqttSub::init() {
    client->setCallback(MqttSub::callback); // Définit le callback pour les messages entrants
    //client->subscribe("homeassistant/#"); // Exemple d'abonnement
}

void MqttSub::loop() {
    client->loop(); // Gère les messages entrants
}

void MqttSub::callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message reçu sur le topic : ");
    Serial.println(topic);

    Serial.print("Payload : ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}
