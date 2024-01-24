#ifndef __MQTT_H_
#define __MQTT_H_

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

class Mqtt {
    public:
        Mqtt();
        void init();

    private:
        PubSubClient* client;

};

#endif  //__MQTT_H_