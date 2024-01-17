#ifndef LOWPOWER_H
#define LOWPOWER_H

#include <Arduino.h>
#include <driver/adc.h>
#include <esp_wifi.h>

extern void printWakeupReason();
extern void deepSleep(int seconds);

#endif