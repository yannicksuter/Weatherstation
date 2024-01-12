#ifndef ANEMOMETER_H
#define ANEMOMETER_H

class Anemometer {
    public:
        Anemometer() = default;
        ~Anemometer() = default;

        void setup(int dir_pin);
        void readWindDirection();

    private:
        int _dirPin;
};

#endif