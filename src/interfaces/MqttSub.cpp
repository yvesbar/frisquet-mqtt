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


    client->subscribe((MqttSub::getDemandeAssociationConnectTopic() + "/#").c_str()); // Exemple d'abonnement
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

    //TODO A tester + envoyer la config HA pour autogénérer le switch (en dur)
    if (MqttSub::getDemandeAssociationConnectTopic().equals(topic)) {
        // Traiter le message spécifique à l'association Frisquet Connect
        //VisioConnectPub::getInstance()->associerFrisquetConnect();

        //Remet l'interrupteur à 0 (!!!!!!!!!!! 
        
         // !!!!!!!!!!génère une boucle infinie)
         // !!!!!!!!!!!
        client->publish((MqttSub::getDemandeAssociationConnectTopic() + "/state").c_str(), "0");

    }
    //Identifier l'action et le payload et appeler VisioConnectPub pour traiter l'action. Soit avec des callback, soit des appels directs

}

const String MqttSub::getDemandeAssociationConnectTopic() {
    return mqttRootNode + "/connect/association";
}


