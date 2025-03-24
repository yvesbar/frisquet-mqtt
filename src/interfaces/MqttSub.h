#ifndef __MQTT_SUB_H_
#define __MQTT_SUB_H_

#include <Arduino.h>
#include <PubSubClient.h>
#include "MqttPub.h"

class MqttSub {
    public:
        MqttSub(PubSubClient* client); // Constructeur avec MqttPub
        void init(); // Initialise les abonnements MQTT
        void loop(); // Gère les messages entrants

    private:
        PubSubClient* client; // Client MQTT partagé
        static void callback(char* topic, byte* payload, unsigned int length); // Callback pour les messages MQTT
};

#endif // __MQTT_SUB_H_
