#include "Mqtt.h"

//Déclaration des constantes
const String Mqtt::MQTT_HA_TOPIC_SENSOR = "homeassistant/sensor/frisquet/";
const String Mqtt::MQTT_HA_TOPIC_SELECT = "homeassistant/select/frisquet/";

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
void Mqtt::deployAutoDiscoveryHA(Chaudiere* chaudiere) {
    if (!configHADeployed) {
        DEBUGLN(F("MQTT - +deployAutoDiscoveryHA"));
        configHADeployed = true;

        deployConfHAZone(chaudiere->getZone1());
        deployConfHAZone(chaudiere->getZone2());
        deployConfHAZone(chaudiere->getZone3());
        
        //TODO à tester
        deployConfHAtempExt();
        
        //TODO à tester :
        deployConfHAChaudiere();

        DEBUGLN(F("MQTT - -deployAutoDiscoveryHA"));
    }
}

/**
* Déploie la confi HA nécessair pour l'auto-configuration d'une zone
*/
void Mqtt::deployConfHAZone(Zone* zone) {
    
    //Seulement si la zone est définie
    if (zone != nullptr) {
        DEBUGLN(F("MQTT - +deployConfHAZone"));
        stringstream ss;
    
        // Configuration du capteur de température ambiante
        ss << "{";
        ss << "\"uniq_id\": \"frisquet_" << zone->getNom().c_str() << "_tempAmbiante\",";
        ss << "\"name\": \"Frisquet - " << zone->getNom().c_str() << " - Temperature ambiante\",";
        ss << "\"state_topic\": \"" << getZoneTempAmbianteTopic(zone->getNom()) << "\",";
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
        ss << "\"state_topic\": \"" << getZoneTempConsigneTopic(zone->getNom()) << "\",";
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
    
        DEBUGLN(F("MQTT - -deployConfHAZone"));
    }
}

/**
* Déploie la Configuration MQTT pour HA pour le capteur de température extérieure
*/
void Mqtt::deployConfHAtempExt() {
    DEBUGLN(F("MQTT - +deployConfHAtempExt"));
    

    //FIXME ajouter le canal "set" si sonde émulé
    if (TEMP_EXTERIEUR_PHYSIQUE_ACTIF) {
        stringstream ss;

        // Configuration du capteur de température ambiante
        ss << "{";
        ss << "\"uniq_id\": \"frisquet_tempExterieure\",";
        ss << "\"name\": \"Frisquet - Temperature extérieure\",";
        ss << "\"state_topic\": \"" << getZoneTempExterieurTopic() << "\",";
        ss << "\"unit_of_measurement\": \"°C\",";
        ss << "\"device_class\": \"temperature\",";
        ss << MQTT_HA_DEVICE_ID;
        ss << "}";
        client->publish((Mqtt::MQTT_HA_TOPIC_SENSOR + "tempExterieure/config").c_str(), ss.str().c_str());
        ss.str("");
    }

    DEBUGLN(F("MQTT - -deployConfHAtempExt"));
}


void Mqtt::deployConfHAChaudiere() {
    DEBUGLN(F("MQTT - +deployConfHAChaudiere"));
    stringstream ss;

    //Publication de la consommation de gaz pour le chauffage
    ss << "{";
    ss << "\"uniq_id\": \"frisquet_consogaz-ch\",";
    ss << "\"name\": \"Frisquet - consommation gaz chauffage\",";
    ss << "\"state_topic\": \"" << getConsoGazChauffageTopic() << "\",";
    ss << "\"unit_of_measurement\": \"kWh\",";
    ss << "\"device_class\": \"energy\",";
    ss << "\"state_class\": \"total_increasing\",";
    ss << MQTT_HA_DEVICE_ID;
    ss << "}";
    client->publish((Mqtt::MQTT_HA_TOPIC_SENSOR + "consogaz-ch/config").c_str(), ss.str().c_str());
    ss.str("");


    //Publication de la consommation de gaz pour l'eau chaude sanitaire
    if (ECS_ACTIF == true) { 
        ss << "{";
        ss << "\"uniq_id\": \"frisquet_consogaz-ecs\",";
        ss << "\"name\": \"Frisquet - consommation gaz eau chaude sanitaire\",";
        ss << "\"state_topic\": \"" << getConsoGazECSTopic() << "\",";
        ss << "\"unit_of_measurement\": \"kWh\",";
        ss << "\"device_class\": \"energy\",";
        ss << "\"state_class\": \"total_increasing\",";
        ss << MQTT_HA_DEVICE_ID;
        ss << "}";
        client->publish((Mqtt::MQTT_HA_TOPIC_SENSOR + "consogaz-ecs/config").c_str(), ss.str().c_str());
        ss.str("");
    }

    // Publication de la température du corps de chauffe
    ss << "{";
    ss << "\"uniq_id\": \"frisquet_tempCorpsDeChauffe\",";
    ss << "\"name\": \"Frisquet - Température corps de chauffe\",";
    ss << "\"state_topic\": \"" << getTempCorpsDeChauffeTopic() << "\",";
    ss << "\"unit_of_measurement\": \"°C\",";
    ss << "\"device_class\": \"temperature\",";
    ss << MQTT_HA_DEVICE_ID;
    ss << "}";
    client->publish((Mqtt::MQTT_HA_TOPIC_SENSOR + "tempCorpsDeChauffe/config").c_str(), ss.str().c_str());
    ss.str("");

  DEBUGLN(F("MQTT - -deployConfHAChaudiere"));
}


void Mqtt::publishAsync(String topic, String value) {
    //FIXME A implémenter lors de l'utilisation des 2 coeurs du heltec
    //mqttPublishQueue->push(MqttPublishEvent(topic, value));
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

String Mqtt::getZoneTempConsigneTopic(String nomZoneMqtt) {
    return mqttRootNode + "/" +  nomZoneMqtt + "/tempConsigne/state";
}

String Mqtt::getZoneTempAmbianteTopic(String nomZoneMqtt) {
    return  mqttRootNode + "/" + nomZoneMqtt + "/tempAmbiante/state";
}

String Mqtt::getZoneTempExterieurTopic() {
    return mqttRootNode + "/tempExterieure/state";
}

String Mqtt::getConsoGazChauffageTopic() {
    return mqttRootNode + "/consogaz-ch/state";
}

String Mqtt::getConsoGazECSTopic() {
    return mqttRootNode + "/consogaz-ecs/state";
}

String Mqtt::getTempCorpsDeChauffeTopic() {
    return mqttRootNode + "/tempCorpsDeChauffe/state";
}

void Mqtt::loop() {
    DEBUGLN(F("MQTT - +loop"));
    if (!client->connected()) {
        DEBUGLN(F("MQTT - Client déconnecté, tentative de reconnexion..."));
        init(); // Réinitialise la connexion MQTT si déconnecté
    }
    client->loop(); // Appelle la boucle MQTT pour gérer les messages entrants et sortants
    DEBUGLN(F("MQTT - -loop"));
}