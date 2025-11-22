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
        void deployConfHAZone(Zone* zone);

        //Stock un évènement pour le publiqué plus tard
        //void publishAsync(String topic, String value);

        //Publication des évènements en attente
        //void publishEvents();
        PubSubClient* getClient(); // Ajout de la méthode getClient

        void publishZoneTempAmbiante(Zone* zone, float temperature, bool force = false); // Publie la température ambiante
        void publishZoneTempConsigne(Zone* zone, float temperature, bool force = false); // Publie la température de consigne
        void publishZoneTempDepart(Zone* zone, float temperature, bool force = false);
        void publishZoneMode(Zone* zone);

        void publishTempExterieure(float temperature); // Publie la température extérieure
        void publishTempECS(float temperature);
        void publishTempCorpsDeChauffe(float temperature);
        void publishConsoGazCh(int value);
        void publishConsoGazECS(int value);
        
        static String getZoneTempConsigneTopic(String nomZoneMqtt);
        static String getZoneTempAmbianteTopic(String nomZoneMqtt);
        static String getZoneTempExterieurTopic();
        static String getConsoGazChauffageTopic();
        static String getConsoGazECSTopic();
        static String getTempECSTopic();
        static String getTempCorpsDeChauffeTopic();
        static String getZoneTempDepartTopic(String nomZoneMqtt);
        static String getZoneModeStateTopic(String nomZoneMqtt);
        static String getZoneModeCommandTopic(String nomZoneMqtt);

    private:
        static const String MQTT_HA_TOPIC_SENSOR;
        static const String MQTT_HA_TOPIC_SELECT;
        
        PubSubClient* client;
        bool configHADeployed = false;
        unsigned long lastZonesPublish = 0; // Timestamp dernière publication horaire
        void publishAllZones(); // Publication de toutes les zones connues
        //TODO a utiliser plus tard quand on utilisera les 2 coeurs du heltec
        //TSQueue<MqttPublishEvent>* mqttPublishQueue;

        void deployConfHAChaudiere();
        void deployConfHAtempExt();
        void deployConfHASwitchAssociationConnect();

        // Helper uniforme pour publier un float avec 1 décimale
        void publishFloat(const String& topic, float value, uint8_t decimals = 1, bool retain = true, bool filter = true);

        // --- Anti-spam ---
        struct TopicState {
            String topic;
            float lastValue = 0.0f;
            unsigned long lastTs = 0; // millis du dernier publish
            bool initialized = false;
        };
        static const uint8_t MAX_TOPIC_STATES = 16;
        TopicState topicStates[MAX_TOPIC_STATES];
        uint8_t topicStateCount = 0;
        float antiSpamSeuil = 0.05f;              // Variation minimale (ex: 0.05 => 0.1°C suffira)
        unsigned long antiSpamMinIntervalMs = 60000UL; // 60s entre deux publications si variation faible

        TopicState* getOrCreateTopicState(const String& topic);
        bool shouldPublishFiltered(TopicState* state, float newValue);

    public:
        // Permet d'ajuster dynamiquement le filtre anti-spam (threshold en °C, intervalle ms)
        void setAntiSpamParameters(float threshold, unsigned long minIntervalMs) {
            antiSpamSeuil = threshold;
            antiSpamMinIntervalMs = minIntervalMs;
        }
};

#endif  //__MQTT_PUB_H_