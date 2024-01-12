#include "S_S35770.h"

void S_S35770::setup(uint8_t addr) {
    S35770_i2caddr = addr;

    // reset();
}

uint32_t S_S35770::readCount() {
    Wire.requestFrom(S35770_i2caddr, (uint8_t)3);
    uint8_t b1 = Wire.read();
    uint8_t b2 = Wire.read();
    uint8_t b3 = Wire.read();

    uint32_t retval=(b1<<16)+(b2 << 8) + b3;

    return retval;
}

void S_S35770::reset() {
    Wire.beginTransmission(S35770_i2caddr);
    Wire.write(0b10000001);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.write(0b00000010);
    Wire.endTransmission();
}

void S_S35770::read() {
  uint32_t cnt = readCount();

  char strBuf[50];
  sprintf(strBuf, "S35770 value = %d", cnt);
  Serial.println(strBuf);
}
