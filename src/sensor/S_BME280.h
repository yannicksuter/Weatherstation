#ifndef SBME280_H
#define SBME280_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "weather.h"

class S_BME280 {
    public:
        S_BME280() = default;
        ~S_BME280() = default;

    void setup(uint8_t addr);
    void collectData(sensor_data_t *data);

private:
    Adafruit_BME280 bme; 
};

#endif