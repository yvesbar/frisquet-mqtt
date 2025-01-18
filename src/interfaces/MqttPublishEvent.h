#ifndef __MQTT_PUBLISH_EVENT_H__
#define __MQTT_PUBLISH_EVENT_H__

#include <string>

class MqttPublishEvent {
    public:
        MqttPublishEvent(std::string topic, std::string value) : topic{topic}, value{value} {}
        std::string getTopic() { return topic;};
        std::string getValue() { return value;};

    private : 
        std::string topic, value;
};

#endif