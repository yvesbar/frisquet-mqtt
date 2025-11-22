#include "MqttPub.h"
#include "MqttSub.h"
#include "modele/Chaudiere.h"

//Déclaration des constantes
const String MqttPub::MQTT_HA_TOPIC_SENSOR = "homeassistant/sensor/frisquet/";
const String MqttPub::MQTT_HA_TOPIC_SELECT = "homeassistant/select/frisquet/";

MqttPub::MqttPub() {
    WiFiClient* wifi = new WiFiClient();
    client = new PubSubClient(*wifi);
    //mqttPublishQueue = new TSQueue<MqttPublishEvent>();
}

void MqttPub::init() {
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
void MqttPub::deployAutoDiscoveryHA() {
    if (!configHADeployed) {
        DEBUGLN(F("MQTT - +deployAutoDiscoveryHA"));
        configHADeployed = true;

        // Fait automatiquement lors de la création des zones
        //deployConfHAZone(chaudiere->getZone1());
        //deployConfHAZone(chaudiere->getZone2());
        //deployConfHAZone(chaudiere->getZone3());
        
        deployConfHAtempExt();
        deployConfHAChaudiere();
        deployConfHASwitchAssociationConnect();

        DEBUGLN(F("MQTT - -deployAutoDiscoveryHA"));
    }
}

/**
* Déploie la confi HA nécessair pour l'auto-configuration d'une zone
*/
void MqttPub::deployConfHAZone(Zone* zone) {
    //Seulement si la zone est définie
    if (zone != nullptr) {
        DEBUGLN(F("MQTT - +deployConfHAZone"));
        stringstream ss;
    
        // Configuration du capteur de température ambiante
        ss << "{";
        ss << "\"uniq_id\": \"frisquet_" << zone->getNom().c_str() << "_tempAmbiante\",";
        ss << "\"name\": \"Frisquet - " << zone->getNom().c_str() << " - Temperature ambiante\",";
        ss << "\"state_topic\": \"" << getZoneTempAmbianteTopic(zone->getNom()).c_str() << "\",";
        ss << "\"unit_of_measurement\": \"°C\",";
        ss << "\"device_class\": \"temperature\",";
        ss << MQTT_HA_DEVICE_ID;
        ss << "}";
        client->publish((MQTT_HA_TOPIC_SENSOR + zone->getNom() + "-tempAmbiante/config").c_str(), ss.str().c_str(), true);
        ss.str("");
    
        // Configuration du capteur de consigne
        ss << "{";
        ss << "\"uniq_id\": \"frisquet_" << zone->getNom().c_str() << "_tempConsigne\",";
        ss << "\"name\": \"Frisquet - " << zone->getNom().c_str() << " Temperature consigne\",";
        ss << "\"state_topic\": \"" << getZoneTempConsigneTopic(zone->getNom()).c_str() << "\",";
        ss << "\"unit_of_measurement\": \"°C\",";
        ss << "\"device_class\": \"temperature\",";
        ss << MQTT_HA_DEVICE_ID;
        ss << "}";
        client->publish((MQTT_HA_TOPIC_SENSOR + zone->getNom() + "-tempConsigne/config").c_str(), ss.str().c_str(), true);
        ss.str("");
    
        // Configuration du mode
        ss << "{";
        ss << "\"uniq_id\": \"frisquet_" << zone->getNom().c_str() << "_mode\",";
        ss << "\"name\": \"Frisquet - " << zone->getNom().c_str() << " mode\",";
        ss << "\"state_topic\": \"" << getZoneModeStateTopic(zone->getNom()).c_str() <<"\",";
        ss << "\"command_topic\": \"" << getZoneModeCommandTopic(zone->getNom()).c_str() <<"\",";
        ss << "\"options\": [\"Auto\", \"Confort\", \"Réduit\", \"Hors gel\"],";
        ss << MQTT_HA_DEVICE_ID;
        ss << "}";
        client->publish((MQTT_HA_TOPIC_SELECT + zone->getNom() + "-mode/config").c_str(), ss.str().c_str(), true);
        ss.str("");
    
        // Configuration du capteur de température de départ
        ss << "{";
        ss << "\"uniq_id\": \"frisquet_" << zone->getNom().c_str() << "_tempDepart\",";
        ss << "\"name\": \"Frisquet - " << zone->getNom().c_str() << " - Temperature départ\",";
        ss << "\"state_topic\": \"" << getZoneTempDepartTopic(zone->getNom()).c_str() << "\",";
        ss << "\"unit_of_measurement\": \"°C\",";
        ss << "\"device_class\": \"temperature\",";
        ss << MQTT_HA_DEVICE_ID;
        ss << "}";
        client->publish((MQTT_HA_TOPIC_SENSOR + zone->getNom() + "-tempDepart/config").c_str(), ss.str().c_str(), true);
        ss.str("");
    
        DEBUGLN(F("MQTT - -deployConfHAZone"));
    }
}

/**
* Déploie la Configuration MQTT pour HA pour le capteur de température extérieure
*/
void MqttPub::deployConfHAtempExt() {
    DEBUGLN(F("MQTT - +deployConfHAtempExt"));
    

    //FIXME ajouter le canal "set" si sonde émulé
    if (TEMP_EXTERIEUR_PHYSIQUE_ACTIF) {
        stringstream ss;

        // Configuration du capteur de température ambiante
        ss << "{";
        ss << "\"uniq_id\": \"frisquet_tempExterieure\",";
        ss << "\"name\": \"Frisquet - Temperature extérieure\",";
        ss << "\"state_topic\": \"" << getZoneTempExterieurTopic().c_str() << "\",";
        ss << "\"unit_of_measurement\": \"°C\",";
        ss << "\"device_class\": \"temperature\",";
        ss << MQTT_HA_DEVICE_ID;
        ss << "}";
        client->publish((MQTT_HA_TOPIC_SENSOR + "tempExterieure/config").c_str(), ss.str().c_str(), true);
        ss.str("");
    }

    DEBUGLN(F("MQTT - -deployConfHAtempExt"));
}


void MqttPub::deployConfHAChaudiere() {
    DEBUGLN(F("MQTT - +deployConfHAChaudiere"));
    stringstream ss;

    //Publication de la consommation de gaz pour le chauffage
    ss << "{";
    ss << "\"uniq_id\": \"frisquet_consogaz-ch\",";
    ss << "\"name\": \"Frisquet - consommation gaz chauffage\",";
    ss << "\"state_topic\": \"" << getConsoGazChauffageTopic().c_str() << "\",";
    ss << "\"unit_of_measurement\": \"kWh\",";
    ss << "\"device_class\": \"energy\",";
    ss << "\"state_class\": \"total_increasing\",";
    ss << MQTT_HA_DEVICE_ID;
    ss << "}";
    client->publish((MQTT_HA_TOPIC_SENSOR + "consogaz-ch/config").c_str(), ss.str().c_str(), true);
    ss.str("");


    //Publication de la consommation de gaz pour l'eau chaude sanitaire
    if (Chaudiere::getInstance().isEcsActif() == true) { 
        ss << "{";
        ss << "\"uniq_id\": \"frisquet_consogaz-ecs\",";
        ss << "\"name\": \"Frisquet - consommation gaz eau chaude sanitaire\",";
        ss << "\"state_topic\": \"" << getConsoGazECSTopic().c_str() << "\",";
        ss << "\"unit_of_measurement\": \"kWh\",";
        ss << "\"device_class\": \"energy\",";
    ss << "\"state_class\": \"total_increasing\",";
        ss << MQTT_HA_DEVICE_ID;
        ss << "}";
        client->publish((MQTT_HA_TOPIC_SENSOR + "consogaz-ecs/config").c_str(), ss.str().c_str(), true);
        ss.str("");
    }

    // Publication de la température du corps de chauffe
    ss << "{";
    ss << "\"uniq_id\": \"frisquet_tempCorpsDeChauffe\",";
    ss << "\"name\": \"Frisquet - Température corps de chauffe\",";
    ss << "\"state_topic\": \"" << getTempCorpsDeChauffeTopic().c_str() << "\",";
    ss << "\"unit_of_measurement\": \"°C\",";
    ss << "\"device_class\": \"temperature\",";
    ss << MQTT_HA_DEVICE_ID;
    ss << "}";
    client->publish((MQTT_HA_TOPIC_SENSOR + "tempCorpsDeChauffe/config").c_str(), ss.str().c_str(), true);
    ss.str("");

    // Publication de la température du ballon ECS
    ss << "{";
    ss << "\"uniq_id\": \"frisquet_tempECS\",";
    ss << "\"name\": \"Frisquet - Température ECS\",";
    ss << "\"state_topic\": \"" << getTempECSTopic().c_str() << "\",";
    ss << "\"unit_of_measurement\": \"°C\",";
    ss << "\"device_class\": \"temperature\",";
    ss << MQTT_HA_DEVICE_ID;
    ss << "}";
    client->publish((MQTT_HA_TOPIC_SENSOR + "tempECS/config").c_str(), ss.str().c_str(), true);
    ss.str("");

  DEBUGLN(F("MQTT - -deployConfHAChaudiere"));
}

void MqttPub::deployConfHASwitchAssociationConnect() {
    DEBUGLN(F("MQTT - +deployConfHASwitchAssociationConnect"));
    stringstream ss;
    ss << "{";
    ss << "\"uniq_id\": \"frisquet_association_switch\",";
    ss << "\"name\": \"Association connect\",";
    ss << "\"command_topic\": \"" << MqttSub::getDemandeAssociationConnectCommandTopic().c_str() << "\",";
    ss << "\"state_topic\": \"" << MqttSub::getDemandeAssociationConnectStateTopic().c_str() << "\",";
    ss << "\"payload_on\": \"1\",";
    ss << "\"payload_off\": \"0\",";
    ss << MQTT_HA_DEVICE_ID;
    ss << "}";
    client->publish("homeassistant/switch/frisquet/association/config", ss.str().c_str(), true);
    ss.str("");
    DEBUGLN(F("MQTT - -deployConfHASwitchAssociationConnect"));
}


/*void MqttPub::publishAsync(String topic, String value) {
    //FIXME A implémenter lors de l'utilisation des 2 coeurs du heltec
    //mqttPublishQueue->push(MqttPublishEvent(topic, value));



/**
 * Publie les évènement en attente sur la file correspondante
*/
/*void MqttPub::publishEvents() {
    DEBUGLN(F("MQTT - +publishEvents"));

    while (!mqttPublishQueue->empty()){
        MqttPublishEvent event = mqttPublishQueue->pop();
        if (!client->publish(event.getTopic().c_str(), event.getValue().c_str())) {
            DEBUGLN(F("Failed to publish to MQTT "));
        }

    }

    DEBUGLN(F("MQTT - -publishEvents"));
}*/


String MqttPub::getZoneTempConsigneTopic(String nomZoneMqtt) {
    return mqttRootNode + "/" +  nomZoneMqtt + "/tempConsigne";
}

String MqttPub::getZoneTempAmbianteTopic(String nomZoneMqtt) {
    return  mqttRootNode + "/" + nomZoneMqtt + "/tempAmbiante";
}

String MqttPub::getZoneTempExterieurTopic() {
    return mqttRootNode + "/tempExterieure";
}

String MqttPub::getConsoGazChauffageTopic() {
    return mqttRootNode + "/consogaz/chauffage";
}

String MqttPub::getConsoGazECSTopic() {
    return mqttRootNode + "/consogaz/ecs";
}

String MqttPub::getTempECSTopic() {
    return mqttRootNode + "/tempECS";
}

String MqttPub::getTempCorpsDeChauffeTopic() {
    return mqttRootNode + "/tempCorpsDeChauffe";
}

String MqttPub::getZoneTempDepartTopic(String nomZoneMqtt) {
    return mqttRootNode + "/" + nomZoneMqtt + "/tempDepart";
}

String MqttPub::getZoneModeStateTopic(String nomZoneMqtt) {
    return mqttRootNode + "/" + nomZoneMqtt + "/mode/state";
}

String MqttPub::getZoneModeCommandTopic(String nomZoneMqtt) {
    return mqttRootNode + "/" + nomZoneMqtt + "/mode/set";
}

void MqttPub::loop() {
    if (!client->connected()) {
        DEBUGLN(F("MQTT - Client déconnecté, tentative de reconnexion..."));
        init(); // Réinitialise la connexion MQTT si déconnecté
    }
    client->loop(); // Appelle la boucle MQTT pour gérer les messages entrants et sortants

    // Publication toutes les heures, permet d'avoir les valeur dans HA après un redémarrage
    // ou si le broker MQTT est redémarré
    unsigned long now = millis();
    if (now - lastZonesPublish > 3600000UL || lastZonesPublish == 0) { // 1h = 3600000 ms
        publishAllZones();
        lastZonesPublish = now;
    }
}

PubSubClient* MqttPub::getClient() {
    return client;
}

// Helper centralisé pour formater les floats avec 1 décimale
MqttPub::TopicState* MqttPub::getOrCreateTopicState(const String& topic) {
    for (uint8_t i = 0; i < topicStateCount; ++i) {
        if (topicStates[i].topic == topic) {
            return &topicStates[i];
        }
    }
    if (topicStateCount < MAX_TOPIC_STATES) {
        topicStates[topicStateCount].topic = topic;
        topicStates[topicStateCount].initialized = false;
        return &topicStates[topicStateCount++];
    }
    return nullptr; // Plus de place, on désactive le filtrage pour ce topic
}

bool MqttPub::shouldPublishFiltered(TopicState* state, float newValue) {
    unsigned long now = millis();
    if (state == nullptr) return true; // Pas de suivi => on publie
    if (!state->initialized) return true; // Première valeur => on publie

    float delta = fabs(newValue - state->lastValue);
    if (delta >= antiSpamSeuil) {
        return true; // Variation suffisante
    }
    // Variation faible, on ne republie que si assez de temps écoulé
    if (now - state->lastTs >= antiSpamMinIntervalMs) {
        return true;
    }
    return false;
}

void MqttPub::publishFloat(const String& topic, float value, uint8_t decimals, bool retain, bool filter) {
    TopicState* state = nullptr;
    if (filter) {
        state = getOrCreateTopicState(topic);
        if (!shouldPublishFiltered(state, value)) {
            return; // filtré
        }
    }
    char payload[12];
    dtostrf(value, 0, decimals, payload);
    if (client->publish(topic.c_str(), payload, retain) && state != nullptr) {
        state->lastValue = value;
        state->lastTs = millis();
        state->initialized = true;
    } else if (state != nullptr && !state->initialized) {
        // Même si publish échoue, on n'initialise pas pour réessayer la prochaine fois
    }
}

void MqttPub::publishZoneTempAmbiante(Zone* zone, float temperature, bool force) {
    String topic = getZoneTempAmbianteTopic(zone->getNom());
    publishFloat(topic, temperature, 1, true, !force);
}

void MqttPub::publishZoneTempConsigne(Zone* zone, float temperature, bool force) {
    String topic = getZoneTempConsigneTopic(zone->getNom());
    publishFloat(topic, temperature, 1, true, !force);
}

void MqttPub::publishZoneTempDepart(Zone* zone, float temperature, bool force) {
    String topic = getZoneTempDepartTopic(zone->getNom());
    publishFloat(topic, temperature, 1, true, !force);
}

void MqttPub::publishTempExterieure(float temperature) {
    if (temperature < 0 || temperature >= 0.1) {
        String topic = getZoneTempExterieurTopic();
        publishFloat(topic, temperature, 1, true, true);
    }
}

void MqttPub::publishTempECS(float temperature) {
    publishFloat(getTempECSTopic(), temperature, 1, true, true);
}

void MqttPub::publishTempCorpsDeChauffe(float temperature) {
    publishFloat(getTempCorpsDeChauffeTopic(), temperature, 1, true, true);
}

void MqttPub::publishConsoGazCh(int value) {
    char payload[10];
    snprintf(payload, sizeof(payload), "%d", value);
    client->publish(getConsoGazChauffageTopic().c_str(), payload, true);
}

void MqttPub::publishConsoGazECS(int value) {
    char payload[10];
    snprintf(payload, sizeof(payload), "%d", value);
    client->publish(getConsoGazECSTopic().c_str(), payload, true);
}


/**
 * Publie le mode de la zone dans le topic approprié
 * @param zone Pointeur vers l'objet Zone dont on veut publier le mode
 */
void MqttPub::publishZoneMode(Zone* zone) {
    DEBUGLN(F("MQTT - +publishZoneMode"));
    String modeStr;
    switch (zone->getMode()) {
        case Zone::CONFORT:
            modeStr = "Confort";
            break;
        case Zone::REDUIT:
            modeStr = "Réduit";
            break;
        case Zone::HORS_GEL:
            modeStr = "Hors gel";
            break;
        case Zone::AUTO:
        default:
            modeStr = "Auto";
            break;
    }
    client->publish(getZoneModeStateTopic(zone->getNom()).c_str(), modeStr.c_str(), true);
}

void MqttPub::publishAllZones() {
    Chaudiere& chaudiere = Chaudiere::getInstance();
    // Parcours toutes les zones connues
    Zone* zones[3] = {
        chaudiere.getZoneById(Zone::ZONE1_ID),
        chaudiere.getZoneById(Zone::ZONE2_ID),
        chaudiere.getZoneById(Zone::ZONE3_ID)
    };

    for (int i = 0; i < 3; ++i) {
        Zone* zone = zones[i];
        if (zone != nullptr) {
            publishZoneTempAmbiante(zone, zone->getTempAmbiance(), true); // force pour publication horaire
            publishZoneTempConsigne(zone, zone->getTempConsigne(), true);
            publishZoneTempDepart(zone, zone->getTempDepart(), true);
            publishZoneMode(zone);
        }
    }

}