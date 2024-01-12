#ifndef WEATHER_H_
#define WEATHER_H_

#include <Arduino.h>

typedef struct sensor_data_t {
    float temperature;
    float pressure;
    float humidity;

    String wind_dir;
    uint16_t wind_heading;
    float wind_speed_ms;
    float wind_speed_kmh;
    
    float rain;

    float shuntvoltage1;
    float busvoltage1;
    float current_mA1;
    float loadvoltage1;

    float shuntvoltage2;
    float busvoltage2;
    float current_mA2;
    float loadvoltage2;

    float shuntvoltage3;
    float busvoltage3;
    float current_mA3;
    float loadvoltage3;

} sensor_data_t;


inline void debugMessage(const char *format, ...)
{
	static char buffer[256];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

    Serial.println(buffer);    
}

#endif