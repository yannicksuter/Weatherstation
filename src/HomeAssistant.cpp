#include "HomeAssistant.h"
#include "secrets.h"

HomeAssistant::HomeAssistant() {
    strcpy(_sMQTTBrokerUsername, mqtt_user);
    strcpy(_sMQTTBrokerPassword, mqtt_password);

    _iMQTTBrokerIPAddress = IPAddress(192,168,1,56);

    _lastUptimeUpdateAt = 0;

    _haDevice = nullptr;
    _mqtt = nullptr;
    _uptimeSensor = nullptr;
    
    _temperatureSensor = nullptr;
    _humiditySensor = nullptr;
    _pressureSensor = nullptr;
    
    _rainSensor = nullptr;
}

HomeAssistant::~HomeAssistant() {
    delete _haDevice;
    delete _mqtt;
    delete _uptimeSensor;
    delete _temperatureSensor;
    delete _humiditySensor;
    delete _pressureSensor;
    delete _rainSensor;
}

uint16_t getSerial() {
    uint64_t addr = ESP.getEfuseMac();
    uint8_t serial[8];
    memcpy(serial, &addr, sizeof(addr));

    uint8_t serialNumber_lowByte = serial[0]+serial[1]+serial[2]+serial[3]; // magic 
    uint8_t serialNumber_highByte = serial[4]+serial[5]+serial[6]+serial[7]; // pony
    uint16_t serialNumber = (serialNumber_highByte<<8) | serialNumber_lowByte;    

    return serialNumber;
}

bool HomeAssistant::publishSensorData(sensor_data_t *data) {
    // initialize WIFI
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.setHostname("weatherstation");
        WiFi.mode(WIFI_STA);
        WiFi.begin(wifi_ssid, wifi_password);

        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
        }
        // debugMessage("IP address: %s", WiFi.localIP().toString().c_str());
    }

    String serial = formatString("weatherstation-%d", getSerial());

    _haDevice = new HADevice(serial.c_str());
    _mqtt = new HAMqtt(client, *_haDevice);   

    _haDevice->setName(serial.c_str());
    _haDevice->setSoftwareVersion("1.0.0");
    _haDevice->setModel("Weatherstation");
    _haDevice->setManufacturer("Yannick Suter");

    _mqtt->begin(_iMQTTBrokerIPAddress, _sMQTTBrokerUsername, _sMQTTBrokerPassword);

    _uptimeSensor = new HASensorNumber("ws_bootcounter");
    _uptimeSensor->setIcon("mdi:restart");
    _uptimeSensor->setName("BootCount");
    _uptimeSensor->setUnitOfMeasurement("");

    _temperatureSensor = new HASensorNumber("ws_tempr", HASensorNumber::PrecisionP2);
    _temperatureSensor->setIcon("mdi:temperature-celsius");
    _temperatureSensor->setName("Temperature");
    _temperatureSensor->setUnitOfMeasurement("°C");

    _rainSensor = new HASensorNumber("ws_rain", HASensorNumber::PrecisionP2);
    _rainSensor->setIcon("mdi:weather-rainy");
    _rainSensor->setName("Rain");
    _rainSensor->setUnitOfMeasurement("mm");

    _humiditySensor = new HASensorNumber("ws_humidity", HASensorNumber::PrecisionP2);
    _humiditySensor->setIcon("mdi:water-percent");
    _humiditySensor->setName("Humidity");
    _humiditySensor->setUnitOfMeasurement("%");

    _pressureSensor = new HASensorNumber("ws_pressure", HASensorNumber::PrecisionP2);
    _pressureSensor->setIcon("mdi:cloud-upload-outline");
    _pressureSensor->setName("Pressure");
    _pressureSensor->setUnitOfMeasurement("hPa");

    _mqtt->loop();
    _uptimeSensor->setValue(data->bootCount);
    _temperatureSensor->setValue((float)data->temperature);
    _humiditySensor->setValue((float)data->humidity);
    _pressureSensor->setValue((float)data->pressure);
    _rainSensor->setValue((float)data->rain);
    
    return _mqtt->isConnected(); 
}
