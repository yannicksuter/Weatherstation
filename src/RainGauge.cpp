#include "RainGauge.h"

void RainGauge::setup(uint8_t addr) {
    s35770.setup(addr);
}

void RainGauge::read() {
    uint32_t cnt = s35770.readCount();
    s35770.reset();
    float rain_mm = 0.2794 * cnt; // since last reading...

    static char strBuf[250];
    sprintf(strBuf, "rain: %f mm since last reading", rain_mm);
    Serial.println(strBuf);
}

