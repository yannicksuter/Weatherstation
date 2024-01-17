#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

// #define ENABLE_SERIAL_DEBUG

#include "LowPower.h"

#include "sensor/S_BME280.h"
#include "sensor/S_INA3221.h"

#include "weather.h"
#include "Anemometer.h"
#include "RainGauge.h"

#include "HomeAssistant.h"

#define ONBOARD_LED GPIO_NUM_2
#define WIND_DIR_PIN GPIO_NUM_35
#define WIND_SPEED_PIN GPIO_NUM_33

uint8_t Addr_s35770=0x32;
uint8_t Addr_INA3221=0x40;
uint8_t Addr_BME280=0x76;

S_INA3221 ina3221;
S_BME280 bme;
Anemometer anemometer;
RainGauge raingauge;

RTC_DATA_ATTR int bootCount = 0;

void printData(sensor_data_t *data) {
    debugMessage("--<Data Dump>---------------------------------");
    debugMessage("Boot count: %d", data->boot_count);

    debugMessage("Wind direction=%s, heading=%d° (adc: %d)", data->wind_dir.c_str(), data->wind_heading, data->wind_dir_adc);
    debugMessage("Wind speed: %.3f m/s", data->wind_speed_ms);
    debugMessage("Wind speed: %.3f km/h", data->wind_speed_kmh);
    debugMessage("Wind scale: %s (%d)", data->wind_scale, data->wind_scale_code);
    debugMessage("Rain: %.3f mm (since last reading)", data->rain);

    debugMessage("Temperatur: %.3f °C", data->temperature);
    debugMessage("Pressure: %.3f hPa", data->pressure);
    debugMessage("Humidity: %.3f %%", data->humidity);

    debugMessage("LIPO => Bus Voltage: %.3f V Shunt Voltage: %.3f mV Load Voltage: %.3f V Current: %.3f mA", data->busvoltage1, data->shuntvoltage1, data->loadvoltage1, data->current_mA1); 
    debugMessage("Solar => Bus Voltage: %.3f V Shunt Voltage: %.3f mV Load Voltage: %.3f V Current: %.3f mA", data->busvoltage2, data->shuntvoltage2, data->loadvoltage2, data->current_mA2); 
    debugMessage("Output => Bus Voltage: %.3f V Shunt Voltage: %.3f mV Load Voltage: %.3f V Current: %.3f mA", data->busvoltage3, data->shuntvoltage3, data->loadvoltage3, data->current_mA3); 
    debugMessage("----------------------------------------------");
}

HomeAssistant homeAssistant;

void collectData(sensor_data_t &data) {
  data.boot_count = (int)(++bootCount);

  ina3221.collectData(&data);
  bme.collectData(&data);
  anemometer.collectData(&data);
  raingauge.collectData(&data);

  printData(&data);
}

void setup() {
#ifdef ENABLE_SERIAL_DEBUG  
  Serial.begin(SERIAL_SPEED);
  while (!Serial);
#endif

  // activate onboard LED to show SOC is active
  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, HIGH);

  // log wakeup reason from deepsleep
  printWakeupReason();

  // setup peripherials
  adc_power_acquire();
  Wire.begin();
  // scanI2C();

  // setup sensors
  ina3221.setup(Addr_INA3221);
  bme.setup(Addr_BME280);
  anemometer.setup(WIND_DIR_PIN, WIND_SPEED_PIN);
  raingauge.setup(Addr_s35770);

  // read sensor data and send it to HA
  sensor_data_t data;
  collectData(data);

  // HA setup
  if (homeAssistant.connect()) {
    debugMessage("Publishing data to MQTT broker.");
    homeAssistant.publishSensorData(&data);
  }

  // enter low power mode
  digitalWrite(ONBOARD_LED, LOW);
  deepSleep(DEEPSLEEP_SECS);
}

void loop() {
  // nothing needed here...
}