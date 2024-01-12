#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include "sensor/S_BME280.h"
#include "sensor/S_INA3221.h"

#include "weather.h"
#include "Anemometer.h"
#include "RainGauge.h"

#define SERIAL_SPEED     115200  // serial baud rate
#define PRINT_DEC_POINTS 3       // decimal points to print

#define WIND_DIR_PIN GPIO_NUM_35
#define WIND_SPEED_PIN GPIO_NUM_33

uint8_t Addr_s35770=0x32;
uint8_t Addr_INA3221=0x40;
uint8_t Addr_BME280=0x76;

S_INA3221 ina3221;
S_BME280 bme;
Anemometer anemometer;
RainGauge raingauge;

void scanI2C() {
  Serial.println("Scanning I2C devices...");

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

void collectSensorData(sensor_data_t *data) {

}

void setup() {
  Serial.begin(SERIAL_SPEED);
  while (!Serial);

  Wire.begin();

  scanI2C();

  ina3221.setup(Addr_INA3221);
  bme.setup(Addr_BME280);

  anemometer.setup(WIND_DIR_PIN, WIND_SPEED_PIN);
  raingauge.setup(Addr_s35770);
}

void printData(sensor_data_t *data) {
    debugMessage("Wind direction=%s, heading=%d°", data->wind_dir.c_str(), data->wind_heading);
    debugMessage("Wind speed: %.3f ms", data->wind_speed_ms);
    debugMessage("Rain: %.3f mm (since last reading)", data->rain);

    debugMessage("Temperatur: %.3f °C", data->temperature);
    debugMessage("Pressure: %.3f hPa", data->pressure);
    debugMessage("Humidity: %.3f %%", data->humidity);

    debugMessage("LIPO => Bus Voltage: %.3f V Shunt Voltage: %.3f mV Load Voltage: %.3f V Current: %.3f mA", data->busvoltage1, data->shuntvoltage1, data->loadvoltage1, data->current_mA1); 
    debugMessage("Solar => Bus Voltage: %.3f V Shunt Voltage: %.3f mV Load Voltage: %.3f V Current: %.3f mA", data->busvoltage2, data->shuntvoltage2, data->loadvoltage2, data->current_mA2); 
    debugMessage("Output => Bus Voltage: %.3f V Shunt Voltage: %.3f mV Load Voltage: %.3f V Current: %.3f mA", data->busvoltage3, data->shuntvoltage3, data->loadvoltage3, data->current_mA3); 
}

void loop() {
    sensor_data_t data;
    ina3221.collectData(&data);
    bme.collectData(&data);
    anemometer.collectData(&data);
    raingauge.collectData(&data);
    printData(&data);
    delay(1000);
}