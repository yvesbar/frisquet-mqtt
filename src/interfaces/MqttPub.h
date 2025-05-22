#ifndef __MQTT_PUB_H_
#define __MQTT_PUB_H_

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <string>
#include <sstream>
#include <queue>
#include "MqttPublishEvent.h"

#include "config.h"
#include "../debug.h"
#include "../TSQueue.h"
#include "../modele/Chaudiere.h"


#define MQTT_HA_DEVICE_ID "\"device\":{\"ids\":[\"FrisquetConnect\"],\"mf\":\"Frisquet\",\"name\":\"Frisquet Connect\",\"mdl\":\"Frisquet Connect\"}"

using namespace std;

class MqttPub {
    public:
        MqttPub();
        void init();
        void loop(); // Gère les tâches MQTT en continu

        //Permet de déployer la configuration d'auto conf HA
        void deployAutoDiscoveryHA();

        //Stock un évènement pour le publiquer plus tard
        //void publishAsync(String topic, String value);

        //Publication des évènements en attente
        //void publishEvents();
        void deployConfHAZone(Zone* zone);
        PubSubClient* getClient(); // Ajout de la méthode getClient
        void publishTempAmbiante(Zone* zone, float temperature); // Publie la température ambiante
        void publishTempConsigne(Zone* zone, float temperature); // Publie la température de consigne
        void publishTempExterieure(float temperature); // Publie la température extérieure

    private:
        static const String MQTT_HA_TOPIC_SENSOR;
        static const String MQTT_HA_TOPIC_SELECT;
        
        PubSubClient* client;
        bool configHADeployed = false;

        //TODO a utiliser plus tard quand on utilisera les 2 coeurs du heltec
        TSQueue<MqttPublishEvent>* mqttPublishQueue;

        void deployConfHAChaudiere();
        void deployConfHAtempExt();
        void deployConfHASwitchAssociationConnect();
        String getZoneTempConsigneTopic(String nomZoneMqtt);
        String getZoneTempAmbianteTopic(String nomZoneMqtt);
        String getZoneTempExterieurTopic();
        String getConsoGazChauffageTopic();
        String getConsoGazECSTopic();
        String getTempCorpsDeChauffeTopic();
};

#endif  //__MQTT_PUB_H_