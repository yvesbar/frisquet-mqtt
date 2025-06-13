#include "MqttPub.h"
#include "MqttSub.h"

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
        
        //TODO à tester
        deployConfHAtempExt();
        
        //TODO à tester :
        deployConfHAChaudiere();
        deployConfHASwitchAssociationConnect();

        DEBUGLN(F("MQTT - -deployAutoDiscoveryHA"));
    }
}

/**
* Déploie la confi HA nécessair pour l'auto-configuration d'une zone
*/
void MqttPub::deployConfHAZone(Zone* zone) {
    DEBUGLN(F("MQTT - +deployConfHAZone"));
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
        client->publish((MQTT_HA_TOPIC_SENSOR + zone->getNom().c_str() + "/tempAmbiante/config").c_str(), ss.str().c_str());
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
        client->publish((MQTT_HA_TOPIC_SENSOR + zone->getNom() + "/tempConsigne/config").c_str(), ss.str().c_str());
        ss.str("");
    
        // Configuration du mode
        ss << "{";
        ss << "\"uniq_id\": \"frisquet_" << zone->getNom().c_str() << "_mode\",";
        ss << "\"name\": \"Frisquet - " << zone->getNom().c_str() << " mode\",";
        ss << "\"state_topic\": \"" << mqttRootNode << "/" << zone->getNom().c_str() << "/mode/state\",";
        ss << "\"command_topic\": \"" << mqttRootNode << "/" << zone->getNom().c_str() << "/mode/set\",";
        ss << "\"options\": [\"Auto\", \"Confort\", \"Réduit\", \"Hors gel\"],";
        ss << MQTT_HA_DEVICE_ID;
        ss << "}";
        client->publish((MQTT_HA_TOPIC_SELECT + zone->getNom() + "/mode/config").c_str(), ss.str().c_str(), true);
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
        client->publish((MQTT_HA_TOPIC_SENSOR + zone->getNom().c_str() + "/tempDepart/config").c_str(), ss.str().c_str());
        ss.str("");
    
        DEBUGLN(F("MQTT - -deployConfHAZone"));
    }
    DEBUGLN(F("MQTT - -deployConfHAZone"));
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
        client->publish((MQTT_HA_TOPIC_SENSOR + "tempExterieure/config").c_str(), ss.str().c_str());
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
    client->publish((MQTT_HA_TOPIC_SENSOR + "consogaz-ch/config").c_str(), ss.str().c_str());
    ss.str("");


    //Publication de la consommation de gaz pour l'eau chaude sanitaire
    if (ECS_ACTIF == true) { 
        ss << "{";
        ss << "\"uniq_id\": \"frisquet_consogaz-ecs\",";
        ss << "\"name\": \"Frisquet - consommation gaz eau chaude sanitaire\",";
        ss << "\"state_topic\": \"" << getConsoGazECSTopic().c_str() << "\",";
        ss << "\"unit_of_measurement\": \"kWh\",";
        ss << "\"device_class\": \"energy\",";
        ss << "\"state_class\": \"total_increasing\",";
    ss << MQTT_HA_DEVICE_ID;
        ss << "}";
        client->publish((MQTT_HA_TOPIC_SENSOR + "consogaz-ecs/config").c_str(), ss.str().c_str());
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
    client->publish((MQTT_HA_TOPIC_SENSOR + "tempCorpsDeChauffe/config").c_str(), ss.str().c_str());
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

String MqttPub::getTempCorpsDeChauffeTopic() {
    return mqttRootNode + "/tempCorpsDeChauffe";
}

String MqttPub::getZoneTempDepartTopic(String nomZoneMqtt) {
    return mqttRootNode + "/" + nomZoneMqtt + "/tempDepart";
}

void MqttPub::loop() {
    if (!client->connected()) {
        DEBUGLN(F("MQTT - Client déconnecté, tentative de reconnexion..."));
        init(); // Réinitialise la connexion MQTT si déconnecté
    }
    client->loop(); // Appelle la boucle MQTT pour gérer les messages entrants et sortants
}

PubSubClient* MqttPub::getClient() {
    return client;
}

void MqttPub::publishTempAmbiante(Zone* zone, float temperature) {
    String topic = getZoneTempAmbianteTopic(zone->getNom());
    String value = String(temperature, 1); // Converti la température en chaîne avec 1 décimale
    client->publish(topic.c_str(), value.c_str());
}

void MqttPub::publishTempConsigne(Zone* zone, float temperature) {
    String topic = getZoneTempConsigneTopic(zone->getNom());
    String value = String(temperature, 1); // Converti la température en chaîne avec 1 décimale
    client->publish(topic.c_str(), value.c_str());
}

void MqttPub::publishTempExterieure(float temperature) {
    String topic = getZoneTempExterieurTopic();
    String value = String(temperature, 1); // Converti la température en chaîne avec 1 décimale
    client->publish(topic.c_str(), value.c_str());
}

void MqttPub::publishTempECS(float temperature) {
    String topic = getConsoGazECSTopic();
    char payload[10];
    snprintf(payload, sizeof(payload), "%.1f", temperature);
    client->publish(topic.c_str(), payload);
}

void MqttPub::publishTempCorpsDeChauffe(float temperature) {
    String topic = getTempCorpsDeChauffeTopic();
    char payload[10];
    snprintf(payload, sizeof(payload), "%.1f", temperature);
    client->publish(topic.c_str(), payload);
}

//TODO associer cette température de départ à la zone 1
void MqttPub::publishTempDepart(float temperature) {
    String topic = "homeassistant/sensor/frisquet/tempDepart";
    char payload[10];
    snprintf(payload, sizeof(payload), "%.1f", temperature);
    client->publish(topic.c_str(), payload);
}

void MqttPub::publishConsoGazCh(int value) {
    char payload[10];
    snprintf(payload, sizeof(payload), "%d", value);
    client->publish(getConsoGazChauffageTopic().c_str(), payload);
}

void MqttPub::publishConsoGazECS(int value) {
    char payload[10];
    snprintf(payload, sizeof(payload), "%d", value);
    client->publish(getConsoGazECSTopic().c_str(), payload);
}