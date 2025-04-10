
#include "interfaces/MqttPub.h"
#include "interfaces/MqttSub.h"
#include "modele/Chaudiere.h"
#include "interfaces/VisioConnectSub.h"
#include "interfaces/ModuleHeltec.h"


MqttPub * mqttPub;
MqttSub * mqttSub;
VisioConnectSub* visioConnectSub;

void setup() {
    DEBUG_INIT

    //TODO init de la chaudière avec network_id, frisquet connect, etc... à partir des constantes de config.h

    ModuleHeltec::initWifi(ssid, password);
    ModuleHeltec::initRadio();
    ModuleHeltec::initAffichage();
    ModuleHeltec::initOTA();

    // Création des connexions MQTT
    mqttPub = new MqttPub();
    mqttPub->init();
    mqttSub = new MqttSub(mqttPub->getClient());
    mqttSub->init();

    // Initialisation de VisioConnectSub
    visioConnectSub = new VisioConnectSub(mqttPub);
    visioConnectSub->init();
}

void loop() {

    mqttPub->deployAutoDiscoveryHA();    
    visioConnectSub->lireTrame();
    
    mqttPub->loop();
    mqttSub->loop();
}