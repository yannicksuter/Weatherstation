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

    bool connect();
    bool disconnect();

    bool publishSensorData(sensor_data_t *data);

private:
    WiFiClient client;
    HADevice *_haDevice;
    HAMqtt *_mqtt;
    String _haDeviceSerial;
    
    HASensorNumber *_bootCounterSensor;
    HASensorNumber *_temperatureSensor;
    HASensorNumber *_humiditySensor;
    HASensorNumber *_pressureSensor;
    HASensor *_windDirSensor;
    HASensorNumber *_windDirADCSensor;
    HASensor *_windScaleSensor;
    HASensorNumber *_windScaleCodeSensor;
    HASensorNumber *_windHeadingSensor;
    HASensorNumber *_windSpeedSensor;
    HASensorNumber *_rainSensor;

    HASensorNumber *_lipoBusVoltageSensor;
    HASensorNumber *_lipoShuntVoltageSensor;
    HASensorNumber *_lipoLoadVoltageSensor;
    HASensorNumber *_lipoCurrentSensor;
    HASensorNumber *_solarBusVoltageSensor;
    HASensorNumber *_solarShuntVoltageSensor;
    HASensorNumber *_solarLoadVoltageSensor;
    HASensorNumber *_solarCurrentSensor;
    HASensorNumber *_outputBusVoltageSensor;
    HASensorNumber *_outputShuntVoltageSensor;
    HASensorNumber *_outputLoadVoltageSensor;
    HASensorNumber *_outputCurrentSensor;

    IPAddress _iMQTTBrokerIPAddress;
    char _sMQTTBrokerUsername[255];
    char _sMQTTBrokerPassword[255];

    uint32_t _lastUptimeUpdateAt;
};

#endif
