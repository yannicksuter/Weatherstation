#include "S_BME280.h"

void S_BME280::setup(uint8_t addr) {
  bool status = bme.begin(addr, &Wire);
  if (!status) {
      Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
      Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
      Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
      Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
      Serial.print("        ID of 0x60 represents a BME 280.\n");
      Serial.print("        ID of 0x61 represents a BME 680.\n");
      while (1) delay(10);
  }
}

void S_BME280::collectData(sensor_data_t *data) {
  data->temperature = bme.readTemperature();
  data->pressure = bme.readPressure() / 100.0F;
  data->humidity = bme.readHumidity();
}
