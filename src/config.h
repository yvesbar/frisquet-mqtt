#ifndef __CONFIG_H_
#define __CONFIG_H_

static const char* ssid = "crashNet";  // Mettre votre SSID Wifi
static const char* password = "bbzLyonVlr";  // Mettre votre mot de passe Wifi

static const char* mqttServer = "192.168.120.101"; // Mettre l'IP du serveur MQTT
static const int mqttPort = 1883;
static const char* mqttUsername = ""; // Mettre le user MQTT
static const char* mqttPassword = ""; // Mettre votre mot de passe MQTT
static const String mqttRootNode = "frisquet"; // noeud MQTT dans lequel publier les valeurs

static uint8_t network_id[] = {0x05, 0xDD, 0xCF, 0x7F}; // Remplacer NN par le network id de la chaudière

//Nom des zones (pour mqtt)
static const String ZONE1_NOM = "premier";
static const String ZONE2_NOM = "rdc";
static const String ZONE3_NOM = "zone3";

//Configuration des zones actives
static const bool ZONE2_ACTIF = true;
static const bool ZONE3_ACTIF = false;

//Indique si la chaudière sert pour la production d'eau chaude sanitaire
static const bool ECS_ACTIF = false;

//Indique si le capteur de température extérieur est présent (true) 
static const bool TEMP_EXTERIEUR_PHYSIQUE_ACTIF = true;

//Indique si le capteur de température ext doit etre émulé
//FIXME A implémenter
static const bool TEMP_EXTERIEUR_EMULE_ACTIF = true;



#endif // CONFIG_H
