#ifndef HOMEASSISTANT_H
#define HOMEASSISTANT_H

#include <Arduino.h>
#include <ArduinoHA.h>
#include <WiFi.h>

#include "weather.h"

class HomeAssistant {

public:
    HomeAssistant();
    ~HomeAssistant();

    bool publishSensorData(sensor_data_t *data);

private:
    WiFiClient client;
    HADevice *_haDevice;
    HAMqtt *_mqtt;
    
    HASensorNumber *_uptimeSensor;
    HASensorNumber *_temperatureSensor;
    HASensorNumber *_humiditySensor;
    HASensorNumber *_pressureSensor;

    HASensorNumber *_rainSensor;

    IPAddress _iMQTTBrokerIPAddress;
    char _sMQTTBrokerUsername[255];
    char _sMQTTBrokerPassword[255];

    uint32_t _lastUptimeUpdateAt;
};

#endif
