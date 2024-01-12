#ifndef SS35770_H
#define SS35770_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

class S_S35770 {
    public:
        S_S35770() = default;
        ~S_S35770() = default;

    void setup(uint8_t addr);
    void read();

    uint32_t readCount();
    void reset();

private:
    uint8_t S35770_i2caddr;
};

#endif