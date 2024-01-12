#include "RainGauge.h"

void RainGauge::setup(uint8_t addr) {
    s35770.setup(addr);
}

void RainGauge::read() {
    s35770.read();
}

