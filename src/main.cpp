#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include <driver/adc.h>
#include <esp_wifi.h>

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

void printWakeupReason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}

void printData(sensor_data_t *data) {
    debugMessage("--<Data Dump>---------------------------------");
    debugMessage("Boot count: %d", data->bootCount);

    debugMessage("Wind direction=%s, heading=%d°", data->wind_dir.c_str(), data->wind_heading);
    debugMessage("Wind speed: %.3f ms", data->wind_speed_ms);
    debugMessage("Rain: %.3f mm (since last reading)", data->rain);

    debugMessage("Temperatur: %.3f °C", data->temperature);
    debugMessage("Pressure: %.3f hPa", data->pressure);
    debugMessage("Humidity: %.3f %%", data->humidity);

    debugMessage("LIPO => Bus Voltage: %.3f V Shunt Voltage: %.3f mV Load Voltage: %.3f V Current: %.3f mA", data->busvoltage1, data->shuntvoltage1, data->loadvoltage1, data->current_mA1); 
    debugMessage("Solar => Bus Voltage: %.3f V Shunt Voltage: %.3f mV Load Voltage: %.3f V Current: %.3f mA", data->busvoltage2, data->shuntvoltage2, data->loadvoltage2, data->current_mA2); 
    debugMessage("Output => Bus Voltage: %.3f V Shunt Voltage: %.3f mV Load Voltage: %.3f V Current: %.3f mA", data->busvoltage3, data->shuntvoltage3, data->loadvoltage3, data->current_mA3); 
    debugMessage("----------------------------------------------");
}

void collectData(bool publishToHA) {
  sensor_data_t data;
  data.bootCount = (int)(++bootCount);

  ina3221.collectData(&data);
  bme.collectData(&data);
  anemometer.collectData(&data);
  raingauge.collectData(&data);

  if (publishToHA) {
    debugMessage("publishing...");
    HomeAssistant homeAssistant;
    homeAssistant.publishSensorData(&data);
  }

  printData(&data);
}

void setup() {
  Serial.begin(SERIAL_SPEED);
  while (!Serial);

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
  collectData(true);

  // deep sleep
  esp_sleep_enable_timer_wakeup(DEEPSLEEP_SECS * uS_TO_S_FACTOR);
  
  // power down ESP and enter deepsleep
  Serial.println("Going to sleep now");
  delay(1000);
  Serial.flush();
  esp_wifi_stop();  
  adc_power_release();
  digitalWrite(ONBOARD_LED, LOW);

  esp_deep_sleep_start();
}

void loop() {
  // nothing needs to be here
}