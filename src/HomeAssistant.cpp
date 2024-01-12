#include <Arduino.h>
#include "HomeAssistant.h"

#include <WiFi.h>

HomeAssistant::HomeAssistant(IPAddress ipAddress, String mqttBrokerUsername, String mqttBrokerPassword) 
{
    strcpy(_sMQTTBrokerUsername, mqttBrokerUsername.c_str());
    strcpy(_sMQTTBrokerPassword, mqttBrokerPassword.c_str());
    
    _iMQTTBrokerIPAddress = ipAddress;

    _lastUptimeUpdateAt = 0;

    _haDevice = nullptr;
    _mqtt = nullptr;
    _uptimeSensor = nullptr;
    _temperatureSensor = nullptr;
}

bool HomeAssistant::setup() {
    if(!_iMQTTBrokerIPAddress) {
        return false;
    }

    const char* _serial = "30242";

    _haDevice = new HADevice(_serial);
    _mqtt = new HAMqtt(client, *_haDevice);   

    _haDevice->setName("weatherstation-30242");
    _haDevice->setSoftwareVersion("1.0.0");
    _haDevice->setModel("Weatherstation");
    _haDevice->setManufacturer("Yannick Suter");

    _mqtt->begin(_iMQTTBrokerIPAddress, _sMQTTBrokerUsername, _sMQTTBrokerPassword);

    _uptimeSensor = new HASensorNumber("ws_uptime");
    _uptimeSensor->setIcon("mdi:airplane-clock");
    _uptimeSensor->setName("Uptime");
    _uptimeSensor->setUnitOfMeasurement("m");

    _temperatureSensor = new HASensorNumber("ws_tempr", HASensorNumber::PrecisionP2);
    _temperatureSensor->setIcon("mdi:home-thermometer");
    _temperatureSensor->setName("Temperature");
    _temperatureSensor->setUnitOfMeasurement("C");

    _mqtt->loop();
    _uptimeSensor->setValue(0);
    _temperatureSensor->setValue((float)0.0);
    
    return _mqtt->isConnected(); 
}

void HomeAssistant::reportTemperature(float value) {
    // Serial.print("HA new temperatur: ");
    // Serial.println(value); 
    if (_temperatureSensor) {
        _temperatureSensor->setValue(value);
    }
}

void HomeAssistant::update() {
    if(_iMQTTBrokerIPAddress) {
        _mqtt->loop();
    }

    if ((millis() - _lastUptimeUpdateAt) > 60000) { // update in 2s interval
        uint16_t uptimeValue = millis() / 60000;
        if (_uptimeSensor) {
            _uptimeSensor->setValue(uptimeValue);
            // Serial.println(uptimeValue);
        }
        _lastUptimeUpdateAt = millis();
    }    
}