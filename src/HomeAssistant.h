#ifndef HOMEASSISTANT_H
#define HOMEASSISTANT_H

#include <Arduino.h>
#include <ArduinoHA.h>
#include <WiFi.h>

class HomeAssistant {

public:
    HomeAssistant(IPAddress ipAddress, String mqttBrokerUsername,String mqttBrokerPassword);
    ~HomeAssistant()  = default;

    static void onBrightnessCommand(uint8_t brightness, HALight* sender);
    static void onStateCommand(bool state, HALight* sender);
    static void onRGBColorCommand(HALight::RGBColor color, HALight* sender);

    bool setup();
    void update();

    void reportTemperature(float value);

private:
    WiFiClient client;
    HADevice *_haDevice;
    HAMqtt *_mqtt;
    // HALight *_light;
    HASensorNumber *_uptimeSensor;
    HASensorNumber *_temperatureSensor;

    IPAddress _iMQTTBrokerIPAddress;
    char _sMQTTBrokerUsername[255];
    char _sMQTTBrokerPassword[255];

    uint32_t _lastUptimeUpdateAt;
};

#endif
