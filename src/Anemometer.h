#ifndef ANEMOMETER_H
#define ANEMOMETER_H

class Anemometer {
    public:
        Anemometer() = default;
        ~Anemometer() = default;

        void setup(int dirPin, int speedPin);
        void readWindDirection();
        void readWindSpeed(int sampling_time_s);

    private:
        int _dirPin;
        int _speedPin;
};

#endif