#include "Mqtt.h"

//Déclaration des constantes
const string Mqtt::MQTT_HA_TOPIC_SENSOR = "homeassistant/sensor/frisquet/";
const string Mqtt::MQTT_HA_TOPIC_SELECT = "homeassistant/select/frisquet/";

Mqtt::Mqtt() {
    WiFiClient* wifi = new WiFiClient();
    client = new PubSubClient(*wifi);
    mqttPublishQueue = new TSQueue<MqttPublishEvent>();
}

void Mqtt::init() {
    DEBUGLN(F("MQTT - +init"));
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
    DEBUGLN(F("MQTT - -init"));
}



/**
 * Permet de déployer les messages/topics necessaires pour la configuration automatique des devices dans HomeAssistant
*/
void Mqtt::deployConfHA() {
    if (!configHADeployed) {
        DEBUGLN(F("MQTT - +deployConfHA"));
        configHADeployed = true;

        deployConfHAChaudiere();
        deployConfHAtempExt();
        deployConfHAZone(MQTT_ZONE1);
        deployConfHAZone(MQTT_ZONE2);
        DEBUGLN(F("MQTT - -deployConfHA"));
    }

}


/**
* Déploie la Configuration MQTT pour HA pour le capteur de température extérieure
*/
void Mqtt::deployConfHAtempExt() {
    DEBUGLN(F("MQTT - +deployConfHAtempExt"));
    stringstream ss;

    //TODO a tester

    // Configuration du capteur de température ambiante
    ss << "{";
    ss << "\"uniq_id\": \"frisquet_tempExterieure\",";
    ss << "\"name\": \"Frisquet - Temperature extérieure\",";
    ss << "\"state_topic\": \"frisquet/tempExterieure/state\",";
    ss << "\"unit_of_measurement\": \"°C\",";
    ss << "\"device_class\": \"temperature\",";
    ss << MQTT_HA_DEVICE_ID;
    ss << "}";
    
    client->publish((Mqtt::MQTT_HA_TOPIC_SENSOR + "tempExterieure/config").c_str(), ss.str().c_str());

    DEBUGLN(F("MQTT - -deployConfHAtempExt"));
}



void Mqtt::deployConfHAChaudiere() {
    //TODO publier la config pour les données iternes (CDC...)

}


/**
* Déploie la confi HA nécessair pour l'auto-configuration d'une zone
*/
void Mqtt::deployConfHAZone(string nomZoneMqtt) {
    DEBUGLN(F("MQTT - +deployConfHAZone"));
    stringstream ss;

    //TODO a tester

    // Configuration du capteur de température ambiante
    ss << "{";
    ss << "\"uniq_id\": \"frisquet_" << nomZoneMqtt << "_tempAmbiante\",";
    ss << "\"name\": \"Frisquet - " << nomZoneMqtt << " Temperature ambiante\",";
    ss << "\"state_topic\": \"" << getZoneTempAmbianteTopic(nomZoneMqtt) << "\",";
    ss << "\"unit_of_measurement\": \"°C\",";
    ss << "\"device_class\": \"temperature\",";
    ss << MQTT_HA_DEVICE_ID;
    ss << "}";
    
    client->publish((MQTT_HA_TOPIC_SENSOR + nomZoneMqtt + "/tempAmbiante/config").c_str(), ss.str().c_str());

    ss.clear();

    // Configuration du capteur de consigne
    ss << "{";
    ss << "\"uniq_id\": \"frisquet_" << nomZoneMqtt << "_tempConsigne\",";
    ss << "\"name\": \"Frisquet - " << nomZoneMqtt << " Temperature consigne\",";
    ss << "\"state_topic\": \"" << getZoneTempConsigneTopic(nomZoneMqtt) << "\",";
    ss << "\"unit_of_measurement\": \"°C\",";
    ss << "\"device_class\": \"temperature\",";
    ss << MQTT_HA_DEVICE_ID;
    ss << "}";
    
    client->publish((MQTT_HA_TOPIC_SENSOR + nomZoneMqtt + "/tempConsigne/config").c_str(), ss.str().c_str());

    ss.clear();

    // Configuration du mode
    ss << "{";
    ss << "\"uniq_id\": \"frisquet_" << nomZoneMqtt << "_mode\",";
    ss << "\"name\": \"Frisquet - " << nomZoneMqtt << " mode\",";
    ss << "\"state_topic\": \"frisquet/" << nomZoneMqtt << "/mode/state\",";
    ss << "\"command_topic\": \"frisquet/" << nomZoneMqtt << "/mode/set\",";
    ss << "\"options\": [\"Auto\", \"Confort\", \"Réduit\", \"Hors gel\"],";
    ss << MQTT_HA_DEVICE_ID;
    ss << "}";

    client->publish((MQTT_HA_TOPIC_SELECT + nomZoneMqtt + "/mode/config").c_str(), ss.str().c_str(), true);

    ss.clear();

    DEBUGLN(F("MQTT - -deployConfHAZone"));
}


void Mqtt::publishAsync(string topic, string value) {
    mqttPublishQueue->push(MqttPublishEvent(topic, value));
}


/**
 * Publie les évènement en attente sur la file correspondante
*/
void Mqtt::publishEvents() {
    DEBUGLN(F("MQTT - +publishEvents"));

    while (!mqttPublishQueue->empty()){
        MqttPublishEvent event = mqttPublishQueue->pop();
        if (!client->publish(event.getTopic().c_str(), event.getValue().c_str())) {
            DEBUGLN(F("Failed to publish to MQTT "));
        }

    }

    DEBUGLN(F("MQTT - -publishEvents"));
}


/*void Mqtt::publishZone(string nomZoneMqtt, string topic, float temperature) {
    DEBUGLN(F("MQTT - +publishZone"));

    publish(getZoneTempAmbianteTopic(nomZoneMqtt), temperature),

    DEBUGLN(F("MQTT - -publishZone"));
}*/

string Mqtt::getZoneTempConsigneTopic(string nomZoneMqtt) {
    return "frisquet/" + nomZoneMqtt + "/tempConsigne/state";
}

string Mqtt::getZoneTempAmbianteTopic(string nomZoneMqtt) {
    return "frisquet/" + nomZoneMqtt + "/tempAmbiante/state";
}