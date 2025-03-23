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
#include "modele/Chaudiere.h"
#include "MqttPublishEvent.h"


#define MQTT_HA_DEVICE_ID "\"device\":{\"ids\":[\"FrisquetConnect\"],\"mf\":\"Frisquet\",\"name\":\"Frisquet Connect\",\"mdl\":\"Frisquet Connect\"}"

using namespace std;

class Mqtt {
    public:
        Mqtt();
        void init();
        void loop(); // Gère les tâches MQTT en continu

        //Permet de déployer la configuration d'auto conf HA
        void deployAutoDiscoveryHA(Chaudiere*);

        //Stock un évènement pour le publiquer plus tard
        void publishAsync(String topic, String value);

        //Publication des évènements en attente
        void publishEvents();


    private:
        static const String MQTT_HA_TOPIC_SENSOR;
        static const String MQTT_HA_TOPIC_SELECT;
        
        PubSubClient* client;
        bool configHADeployed = false;

        //TODO a utiliser plus tard quand on utilisera les 2 coeurs du heltec
        TSQueue<MqttPublishEvent>* mqttPublishQueue;

        void deployConfHAChaudiere();
        void deployConfHAtempExt();
        void deployConfHAZone(Zone* zone);
        String getZoneTempConsigneTopic(String nomZoneMqtt);
        String getZoneTempAmbianteTopic(String nomZoneMqtt);
        String getZoneTempExterieurTopic();
        String getConsoGazChauffageTopic();
        String getConsoGazECSTopic();
        String getTempCorpsDeChauffeTopic();
};

#endif  //__MQTT_H_