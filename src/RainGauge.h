#ifndef RAINGAUGE_H
#define RAINGAUGE_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include "weather.h"
#include "sensor/S_S35770.h"

class RainGauge {
    public:
        RainGauge() = default;
        ~RainGauge() = default;

        void setup(uint8_t addr);
        void collectData(sensor_data_t *data);

    private:
        S_S35770 s35770;
};

#endif