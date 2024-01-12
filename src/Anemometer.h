#ifndef ANEMOMETER_H
#define ANEMOMETER_H

#include "weather.h"

class Anemometer {
    public:
        Anemometer() = default;
        ~Anemometer() = default;

        void setup(int dirPin, int speedPin);
        void collectData(sensor_data_t *data);

    private:
        void readWindDirection(String &dir_name, uint16_t &dir_heading);
        void readWindSpeed(int sampling_time_s, float &wind_speed_ms, float &wind_speed_kmh);

        int _dirPin;
        int _speedPin;
};

#endif