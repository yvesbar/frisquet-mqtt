#ifndef __MQTT_SUB_H_
#define __MQTT_SUB_H_

#include <Arduino.h>
#include <PubSubClient.h>
#include "config.h"
#include "MqttPub.h"
#include "VisioConnectPub.h"

class MqttSub {
    
    public:
        // Delete copy constructor and assignment operator
        MqttSub(const MqttSub&) = delete;
        MqttSub& operator=(const MqttSub&) = delete;
        
        // Static method to get the singleton instance
        static MqttSub& getInstance();
        
        void init(PubSubClient* client); // Initialise les abonnements MQTT
        void loop(); // Gère les messages entrants
        
        static void staticCallback(char* topic, byte* payload, unsigned int length);
        void callback(char* topic, byte* payload, unsigned int length); // Callback pour les messages MQTT
        
        static String getDemandeAssociationConnectTopic();
    
    private:
        static MqttSub* instance; // Static instance pointer
        PubSubClient* client;

        // Private constructor to enforce singleton
        MqttSub();
};

#endif // __MQTT_SUB_H_
