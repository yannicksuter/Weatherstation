#ifndef SINA3221_H
#define SINA3221_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include "SDL_Arduino_INA3221.h"

class S_INA3221 {
    public:
        S_INA3221() = default;
        ~S_INA3221() = default;

    void setup(uint8_t addr);
    void read();

private:
    SDL_Arduino_INA3221 ina3221; 
};

#endif