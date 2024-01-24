#ifndef __CONFIG_H_
#define __CONFIG_H_

static const char* ssid = "ssid wifi";  // Mettre votre SSID Wifi
static const char* password = "wifi password";  // Mettre votre mot de passe Wifi

static const char* mqttServer = "192.168.XXX.XXX"; // Mettre l'IP du serveur MQTT
static const int mqttPort = 1883;
static const char* mqttUsername = "mqttUsername"; // Mettre le user MQTT
static const char* mqttPassword = "mqttPassword"; // Mettre votre mot de passe MQTT

static uint8_t network_id[] = {0xFF, 0xFF, 0xFF, 0xFF}; // Remplacer NN par le network id de la chaudière

#endif // CONFIG_H
