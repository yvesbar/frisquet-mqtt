#include "MqttSub.h"


MqttSub::MqttSub()  {
}

MqttSub& MqttSub::getInstance() {
    static MqttSub instance; // Instance unique de la classe
    return instance;
}

void MqttSub::init(PubSubClient* client) {
    this->client = client;
    // Méthode appelée lorsque le client reçoit un message
    client->setCallback(MqttSub::staticCallback);


    client->subscribe((MqttSub::getDemandeAssociationConnectCommandTopic() + "/#").c_str()); // Exemple d'abonnement
    // Souscription aux changements de mode des 3 zones
    client->subscribe(MqttPub::getZoneModeCommandTopic(ZONE1_NOM).c_str());
    client->subscribe(MqttPub::getZoneModeCommandTopic(ZONE2_NOM).c_str());
    client->subscribe(MqttPub::getZoneModeCommandTopic(ZONE3_NOM).c_str());
}

void MqttSub::loop() {
    client->loop(); // Gère les messages entrants
}

void MqttSub::staticCallback(char* topic, byte* payload, unsigned int length) {
        MqttSub::getInstance().callback(topic, payload, length); // Forward to the instance's callback
}

void MqttSub::callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message reçu sur le topic : ");
    Serial.println(topic);

    Serial.print("Payload : ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    if (strcmp(topic, MqttSub::getDemandeAssociationConnectCommandTopic().c_str()) == 0) {
        // Traiter le message spécifique à l'association Frisquet Connect
        VisioConnectPub::getInstance()->associerFrisquetConnect();

        //Remet l'interrupteur à 0
        client->publish(MqttSub::getDemandeAssociationConnectStateTopic().c_str(), "0");
    }


    // Vérifie si le topic correspond à un changement de mode d'une zone
    String zoneNames[3] = {ZONE1_NOM, ZONE2_NOM, ZONE3_NOM};
    byte zoneIds[3] = {Zone::ZONE1_ID, Zone::ZONE2_ID, Zone::ZONE3_ID};
    for (int i = 0; i < 3; ++i) {
        if (strcmp(topic, MqttPub::getZoneModeCommandTopic(zoneNames[i]).c_str()) == 0) {
            // Trouve la zone concernée
            Zone* zone = Chaudiere::getInstance().getZoneById(zoneIds[i]);
        
            if (zone) {
                // Met à jour le mode de la zone en fonction du payload
                bool modeValid = true;
                switch (payload[0]) { // Utiliser le premier caractère pour optimiser
                    case 'A': // "Auto"
                        zone->setMode(Zone::AUTO);
                        break;
                    case 'C': // "Confort"
                        zone->setMode(Zone::CONFORT);
                        break;
                    case 'R': // "Réduit"
                        zone->setMode(Zone::REDUIT);
                        break;
                    case 'H': // "Hors gel"
                        zone->setMode(Zone::HORS_GEL);
                        break;
                    default:
                        modeValid = false;
                        Serial.println("Mode non reconnu !");
                        break;
                }

                // Demande à VisioConnectPub de publier les nouvelles valeurs de la zone seulement si le mode est valide
                if (modeValid) {
                    VisioConnectPub::getInstance()->envoyerZone(zone);
                }
            }
            break;
        }
    }
}

String MqttSub::getDemandeAssociationConnectCommandTopic() {
    return mqttRootNode + "/connect/association/set";
}

String MqttSub::getDemandeAssociationConnectStateTopic() {
    return mqttRootNode + "/connect/association/state";
}


