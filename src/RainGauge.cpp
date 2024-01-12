#include "RainGauge.h"

void RainGauge::setup(uint8_t addr) {
    s35770.setup(addr);
}

void RainGauge::collectData(sensor_data_t *data) {
    uint32_t cnt = s35770.readCount();
    // Serial.println(cnt);
    s35770.reset();

    data->rain = 0.2794 * cnt; // since last reading...
}

