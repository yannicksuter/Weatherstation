#ifndef WEATHER_H_
#define WEATHER_H_

#include <Arduino.h>

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define DEEPSLEEP_SECS (10*60)

#define SERIAL_SPEED 115200  // serial baud rate
#define PRINT_DEC_POINTS 3       // decimal points to print

typedef struct sensor_data_t {
    uint16_t boot_count;

    float temperature;
    float pressure;
    float humidity;

    String wind_dir;
    uint16_t wind_dir_adc;
    uint16_t wind_heading;
    uint16_t wind_scale_code;
    String wind_scale;
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

inline String formatString(const char *format, ...)
{
	static char buffer[256];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
	return String(buffer);
}

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