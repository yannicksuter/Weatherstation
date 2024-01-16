#include "HomeAssistant.h"
#include "secrets.h"

const char* mqtt_deviceclass_none = "none"; 

// byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
// WiFiClient client;
// HADevice device(mac, sizeof(mac));
// HAMqtt mqtt(client, device, 32);

uint16_t getSerial() {
    uint64_t addr = ESP.getEfuseMac();
    uint8_t serial[8];
    memcpy(serial, &addr, sizeof(addr));

    uint8_t serialNumber_lowByte = serial[0]+serial[1]+serial[2]+serial[3]; // magic 
    uint8_t serialNumber_highByte = serial[4]+serial[5]+serial[6]+serial[7]; // pony
    uint16_t serialNumber = (serialNumber_highByte<<8) | serialNumber_lowByte;    

    return serialNumber;
}

HomeAssistant::HomeAssistant() {
    strcpy(_sMQTTBrokerUsername, mqtt_user);
    strcpy(_sMQTTBrokerPassword, mqtt_password);
    // _iMQTTBrokerIPAddress = IPAddress(192,168,1,56);
    _iMQTTBrokerIPAddress.fromString(mqtt_brokerIP);

    _lastUptimeUpdateAt = 0;
    _haDeviceSerial = formatString("ws-%d", getSerial()); 

    _haDevice = nullptr;
    _mqtt = nullptr;

    _bootCounterSensor = nullptr;
    
    _temperatureSensor = nullptr;
    _humiditySensor = nullptr;
    _pressureSensor = nullptr;
    _windDirSensor = nullptr;
    _windScaleSensor = nullptr;
    _windSpeedSensor = nullptr;
    _windHeadingSensor = nullptr;
    _windSpeedSensor = nullptr;
    _rainSensor = nullptr;

    _lipoBusVoltageSensor = nullptr;
    _lipoShuntVoltageSensor = nullptr;
    _lipoLoadVoltageSensor = nullptr;
    _lipoCurrentSensor = nullptr;
    _solarBusVoltageSensor = nullptr;
    _solarShuntVoltageSensor = nullptr;
    _solarLoadVoltageSensor = nullptr;
    _solarCurrentSensor = nullptr;
    _outputBusVoltageSensor = nullptr;
    _outputShuntVoltageSensor = nullptr;
    _outputLoadVoltageSensor = nullptr;
    _outputCurrentSensor = nullptr;
}

HomeAssistant::~HomeAssistant() {
    delete _mqtt;
    delete _haDevice;

    delete _bootCounterSensor;
    delete _temperatureSensor;
    delete _humiditySensor;
    delete _pressureSensor;
    delete _windDirSensor;
    delete _windScaleSensor;
    delete _windHeadingSensor;
    delete _windSpeedSensor;
    delete _rainSensor;

    delete _lipoBusVoltageSensor;
    delete _lipoShuntVoltageSensor;
    delete _lipoLoadVoltageSensor;
    delete _lipoCurrentSensor;
    delete _solarBusVoltageSensor;
    delete _solarShuntVoltageSensor;
    delete _solarLoadVoltageSensor;
    delete _solarCurrentSensor;
    delete _outputBusVoltageSensor;
    delete _outputShuntVoltageSensor;
    delete _outputLoadVoltageSensor;
    delete _outputCurrentSensor;
}

HASensor *createSensorText(const char* uniqueId, const char* name, const char* icon) {
    HASensor *sensor = new HASensor(uniqueId);
    sensor->setIcon(icon);
    sensor->setName(name);
    return sensor;
}

HASensorNumber *createSensorNumber(const char* uniqueId, const char* name, HABaseDeviceType::NumberPrecision precision, const char* icon, const char* unitOfMeasurement) {    
    HASensorNumber *sensor = new HASensorNumber(uniqueId, precision);
    sensor->setIcon(icon);
    sensor->setName(name);
    sensor->setUnitOfMeasurement(unitOfMeasurement);
    return sensor;
}

bool HomeAssistant::connect() {
    // initialize WIFI
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.setHostname("weatherstation");
        WiFi.mode(WIFI_STA);
        WiFi.begin(wifi_ssid, wifi_password);

        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
        }
        debugMessage("WIFI IP address: %s", WiFi.localIP().toString().c_str());
    }

    debugMessage("Registering MQTT device: %s", _haDeviceSerial.c_str());
    if (_haDevice == nullptr) {
        _haDevice = new HADevice(_haDeviceSerial.c_str());
    }

    if (_mqtt == nullptr) {
        _mqtt = new HAMqtt(client, *_haDevice, 32);   
    }

    _haDevice->setName(_haDeviceSerial.c_str());
    _haDevice->setSoftwareVersion("1.0.0");
    _haDevice->setModel("Weatherstation");
    _haDevice->setManufacturer("Yannick Suter");

    _bootCounterSensor = createSensorNumber("ws_bootcounter", "BootCount", HASensorNumber::PrecisionP0, "mdi:restart", "");
    _temperatureSensor = createSensorNumber("ws_tempr", "Temperature", HASensorNumber::PrecisionP2, "mdi:temperature-celsius", "°C"); //°
    _rainSensor = createSensorNumber("ws_rain", "Rain", HASensorNumber::PrecisionP2, "mdi:weather-rainy", "mm");
    _humiditySensor = createSensorNumber("ws_humidity", "Humidity", HASensorNumber::PrecisionP2, "mdi:water-percent", "%");
    _pressureSensor = createSensorNumber("ws_pressure", "Pressure", HASensorNumber::PrecisionP2, "mdi:gauge", "hPa");
    _windDirSensor = createSensorText("ws_winddir", "Wind Direction", "mdi:eye");
    _windScaleSensor = createSensorText("ws_windscale", "Wind Scale", "mdi:windsock");
    _windHeadingSensor = createSensorNumber("ws_windheading", "Wind Heading", HASensorNumber::PrecisionP2, "mdi:eye", "deg");
    _windSpeedSensor = createSensorNumber("ws_windspeed", "Wind Speed", HASensorNumber::PrecisionP2, "mdi:weather-windy", "m/s");

    _lipoBusVoltageSensor = createSensorNumber("ws_lipobusvoltage", "Lipo: Bus Voltage", HASensorNumber::PrecisionP2, "mdi:sine-wave", "V");
    _lipoShuntVoltageSensor = createSensorNumber("ws_liposhuntvoltage", "Lipo: Shunt Voltage", HASensorNumber::PrecisionP2, "mdi:sine-wave", "mV");
    _lipoLoadVoltageSensor = createSensorNumber("ws_lipoloadvoltage", "Lipo: Load Voltage", HASensorNumber::PrecisionP2, "mdi:sine-wave", "V");
    _lipoCurrentSensor = createSensorNumber("ws_lipocurrent", "Lipo: Current", HASensorNumber::PrecisionP2, "mdi:sine-wave", "mA");

    _solarBusVoltageSensor = createSensorNumber("ws_solarbusvoltage", "Solar: Bus Voltage", HASensorNumber::PrecisionP2, "mdi:sine-wave", "V");
    _solarShuntVoltageSensor = createSensorNumber("ws_solarshuntvoltage", "Solar: Shunt Voltage", HASensorNumber::PrecisionP2, "mdi:sine-wave", "mV");
    _solarLoadVoltageSensor = createSensorNumber("ws_solarloadvoltage", "Solar: Load Voltage", HASensorNumber::PrecisionP3, "mdi:sine-wave", "V");
    _solarCurrentSensor = createSensorNumber("ws_solarcurrent", "Solar: Current", HASensorNumber::PrecisionP3, "mdi:sine-wave", "mA");

    _outputBusVoltageSensor = createSensorNumber("ws_outputbusvoltage", "Output: Bus Voltage", HASensorNumber::PrecisionP3, "mdi:sine-wave", "V");
    _outputShuntVoltageSensor = createSensorNumber("ws_outputshuntvoltage", "Output: Shunt Voltage", HASensorNumber::PrecisionP3, "mdi:sine-wave", "mV");
    _outputLoadVoltageSensor = createSensorNumber("ws_outputloadvoltage", "Output: Load Voltage", HASensorNumber::PrecisionP3, "mdi:sine-wave", "V");
    _outputCurrentSensor = createSensorNumber("ws_outputcurrent", "Output: Current", HASensorNumber::PrecisionP3, "mdi:sine-wave", "mA");

    debugMessage("MQTT setup completed.");

    return _mqtt->begin(_iMQTTBrokerIPAddress, _sMQTTBrokerUsername, _sMQTTBrokerPassword);
}

bool HomeAssistant::disconnect() {
    if (_mqtt) {
        return _mqtt->disconnect();
    }
    return true;
}

bool HomeAssistant::publishSensorData(sensor_data_t *data) {
    _mqtt->loop();
    
    _bootCounterSensor->setValue(data->bootCount);
    _temperatureSensor->setValue((float)data->temperature, true);
    _humiditySensor->setValue((float)data->humidity, true);
    _pressureSensor->setValue((float)data->pressure, true);   
    _rainSensor->setValue((float)data->rain, true);
    _windDirSensor->setValue(data->wind_dir.c_str());
    _windScaleSensor->setValue("Light Breeze");
    _windHeadingSensor->setValue(data->wind_heading, true);
    _windSpeedSensor->setValue((float)data->wind_speed_ms, true);

    _lipoBusVoltageSensor->setValue((float)data->busvoltage1, true);
    _lipoShuntVoltageSensor->setValue((float)data->shuntvoltage1, true);
    _lipoLoadVoltageSensor->setValue((float)data->loadvoltage1, true);
    _lipoCurrentSensor->setValue((float)data->current_mA1, true);

    _solarBusVoltageSensor->setValue((float)data->busvoltage2, true);
    _solarShuntVoltageSensor->setValue((float)data->shuntvoltage2, true);
    _solarLoadVoltageSensor->setValue((float)data->loadvoltage2, true);
    _solarCurrentSensor->setValue((float)data->current_mA2, true);

    _outputBusVoltageSensor->setValue((float)data->busvoltage3, true);
    _outputShuntVoltageSensor->setValue((float)data->shuntvoltage3, true);
    _outputLoadVoltageSensor->setValue((float)data->loadvoltage3, true);
    _outputCurrentSensor->setValue((float)data->current_mA3, true);

    _mqtt->loop();

    return true;
}