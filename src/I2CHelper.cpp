#include <Arduino.h>
#include <Wire.h>

#include "weather.h"

void scanI2C() {
  debugMessage("Scanning I2C devices...");

  byte error, address;
  int devicesCount = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      Serial.println(" !");
      devicesCount++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }

  if (devicesCount == 0) {
    Serial.println("No I2C devices found");
  } else {
    Serial.println("Scan complete");
  }
}
