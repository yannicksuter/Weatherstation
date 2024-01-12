#include "S_INA3221.h"

#define LIPO_BATTERY_CHANNEL 1
#define SOLAR_CELL_CHANNEL 2
#define OUTPUT_CHANNEL 3

void S_INA3221::setup(uint8_t addr) {
  Serial.println("Measuring voltage and current with ina3221 ...");
  ina3221.begin();

  Serial.print("Manufactures ID=0x");
  int MID = ina3221.getManufID();
  Serial.println(MID,HEX);
}

void S_INA3221::collectData(sensor_data_t *data) {
  data->busvoltage1 = ina3221.getBusVoltage_V(LIPO_BATTERY_CHANNEL);
  data->shuntvoltage1 = ina3221.getShuntVoltage_mV(LIPO_BATTERY_CHANNEL);
  data->current_mA1 = -ina3221.getCurrent_mA(LIPO_BATTERY_CHANNEL);  // minus is to get the "sense" right.   - means the battery is charging, + that it is discharging
  data->loadvoltage1 = data->busvoltage1 + (data->shuntvoltage1 / 1000);

  data->busvoltage2 = ina3221.getBusVoltage_V(SOLAR_CELL_CHANNEL);
  data->shuntvoltage2 = ina3221.getShuntVoltage_mV(SOLAR_CELL_CHANNEL);
  data->current_mA2 = -ina3221.getCurrent_mA(SOLAR_CELL_CHANNEL);
  data->loadvoltage2 = data->busvoltage2 + (data->shuntvoltage2 / 1000);

  data->busvoltage3 = ina3221.getBusVoltage_V(OUTPUT_CHANNEL);
  data->shuntvoltage3 = ina3221.getShuntVoltage_mV(OUTPUT_CHANNEL);
  data->current_mA3 = ina3221.getCurrent_mA(OUTPUT_CHANNEL);
  data->loadvoltage3 = data->busvoltage3 + (data->shuntvoltage3 / 1000);
}
