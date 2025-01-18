#ifndef __MQTT_H_
#define __MQTT_H_

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <string>
#include <sstream>
#include <queue>

#include "../debug.h"
#include "../TSQueue.h"
#include "config.h"
#include "MqttPublishEvent.h"


#define MQTT_HA_DEVICE_ID "\"device\":{\"ids\":[\"FrisquetConnect\"],\"mf\":\"Frisquet\",\"name\":\"Frisquet Connect\",\"mdl\":\"Frisquet Connect\"}"
#define MQTT_ZONE1 "zone1"
#define MQTT_ZONE2 "zone2"




using namespace std;

class Mqtt {
    public:
        Mqtt();
        void init();

        //Permet de déployer la configuration d'auto conf HA
        void deployConfHA();

        //Stock un évènement pour le publiquer plus tard
        void publishAsync(string topic, string value);

        //Publication des évènements en attente
        void publishEvents();


    private:
        static const string MQTT_HA_TOPIC_SENSOR;
        static const string MQTT_HA_TOPIC_SELECT;
        
        PubSubClient* client;
        bool configHADeployed = false;
        TSQueue<MqttPublishEvent>* mqttPublishQueue;

        void deployConfHAChaudiere();
        void deployConfHAtempExt();
        void deployConfHAZone(string nomZoneMqtt);
        string getZoneTempConsigneTopic(string nomZoneMqtt);
        string getZoneTempAmbianteTopic(string nomZoneMqtt);
};

#endif  //__MQTT_H_