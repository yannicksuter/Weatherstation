#include "Anemometer.h"

#include <Arduino.h>

#define DIR_THRESHOLD 50
#define INT_FILTER_US 50

bool between(int val, int lower_limit, int upper_limit) {
    return (val > lower_limit && val < upper_limit);
}

bool around(int val, int ref, int threshold) {
    return between(val, ref-threshold, ref+threshold);
}

uint32_t sensor_count_ = 0;
uint32_t sensor_last_intr_ = false;
bool sensor_last_pin_val_ = false;
bool sensor_in_pulse_ = false;

void pulseCounter() {
   const uint32_t now = micros();
   const bool pin_val = (bool)gpio_get_level(GPIO_NUM_33);
 
   // A pulse occurred faster than we can detect
   if (sensor_last_pin_val_ == pin_val) {
     // If we haven't reached the filter length yet we need to reset our last_intr_ to now
     // otherwise we can consider this noise as the "pulse" was certainly less than filter_us_
     if (now - sensor_last_intr_ < INT_FILTER_US) {
       sensor_last_intr_ = now;
     }
   } else {
     // Check if the last interrupt was long enough in the past
     if (now - sensor_last_intr_ > INT_FILTER_US) {
       // High pulse of filter length now falling (therefore last_intr_ was the rising edge)
       if (!sensor_in_pulse_ && sensor_last_pin_val_) {
         sensor_in_pulse_ = true;
       }
       // Low pulse of filter length now rising (therefore last_intr_ was the falling edge)
       else if (sensor_in_pulse_ && !sensor_last_pin_val_) {
         sensor_count_++;
         sensor_in_pulse_ = false;
       }
     }
 
     sensor_last_intr_ = now;
     sensor_last_pin_val_ = pin_val;
   }
}

void Anemometer::setup(int dirPin, int speedPin) {
    _dirPin = dirPin;
    _speedPin = speedPin;

    // wind direction
    analogSetPinAttenuation(_dirPin, ADC_11db);

    // wind speed
    pinMode(_speedPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(_speedPin), pulseCounter, CHANGE);
}

void Anemometer::collectData(sensor_data_t *data) {
    readWindDirection(data->wind_dir, data->wind_heading, data->wind_dir_adc);
    readWindSpeed(10, data->wind_speed_ms, data->wind_speed_kmh);

    if (data->wind_speed_kmh < 1) {
        data->wind_scale_code = 0;
        data->wind_scale = "Calm";
    } else if (data->wind_speed_kmh >= 1 && data->wind_speed_kmh < 6) {
        data->wind_scale_code = 1;
        data->wind_scale = "Light Air";
    } else if (data->wind_speed_kmh >= 6 && data->wind_speed_kmh < 12) {
        data->wind_scale_code = 2;
        data->wind_scale = "Light Breeze";
    } else if (data->wind_speed_kmh >= 12 && data->wind_speed_kmh < 20) {
        data->wind_scale_code = 3;
        data->wind_scale = "Gentle Breeze";
    } else if (data->wind_speed_kmh >= 20 && data->wind_speed_kmh < 29) {
        data->wind_scale_code = 4;
        data->wind_scale = "Moderate Breeze";
    } else if (data->wind_speed_kmh >= 29 && data->wind_speed_kmh < 39) {
        data->wind_scale_code = 5;
        data->wind_scale = "Fresh Breeze";
    } else if (data->wind_speed_kmh >= 39 && data->wind_speed_kmh < 50) {
        data->wind_scale_code = 6;
        data->wind_scale = "Strong Breeze";
    } else if (data->wind_speed_kmh >= 50 && data->wind_speed_kmh < 62) {
        data->wind_scale_code = 7;
        data->wind_scale = "Near Gale";
    } else if (data->wind_speed_kmh >= 62 && data->wind_speed_kmh < 75) {
        data->wind_scale_code = 8;
        data->wind_scale = "Gale";
    } else if (data->wind_speed_kmh >= 75 && data->wind_speed_kmh < 89) {
        data->wind_scale_code = 9;
        data->wind_scale = "Severe Gale";
    } else if (data->wind_speed_kmh >= 89 && data->wind_speed_kmh < 103) {
        data->wind_scale_code = 10;
        data->wind_scale = "Storm";
    } else if (data->wind_speed_kmh >= 103 && data->wind_speed_kmh < 118) {
        data->wind_scale_code = 11;
        data->wind_scale = "Violent Storm";
    } else if (data->wind_speed_kmh >= 118) {
        data->wind_scale_code = 12;
        data->wind_scale = "Hurricane Force";
    }
}

void Anemometer::readWindDirection(String &dir_name, uint16_t &dir_heading, uint16_t &dir_adc) {
    dir_adc = analogRead(_dirPin);
    
    dir_name = "undefined";
    dir_heading = -1;

    // mapping based on a 10kOhm resistor 
    if (around(dir_adc, 2950, DIR_THRESHOLD)) {
        dir_name = "N";
        dir_heading = 0;
    }
    else if (around(dir_adc, 1650, DIR_THRESHOLD)) {
        dir_name = "NE";
        dir_heading = 45;
    }
    else if (around(dir_adc, 175, DIR_THRESHOLD)) {
        dir_name = "E";
        dir_heading = 90;
    }
    else if (around(dir_adc, 550, DIR_THRESHOLD)) {
        dir_name = "SE";
        dir_heading = 135;
    }
    else if (around(dir_adc, 950, DIR_THRESHOLD)) {
        dir_name = "S";
        dir_heading = 180;
    }
    else if (around(dir_adc, 2300, DIR_THRESHOLD)) {
        dir_name = "SW";
        dir_heading = 225;
    }
    else if (around(dir_adc, 3850, DIR_THRESHOLD)) {
        dir_name = "W";
        dir_heading = 270;
    }
    else if (around(dir_adc, 3450, DIR_THRESHOLD)) {
        dir_name = "NW";
        dir_heading = 315;
    } 
}

void Anemometer::readWindSpeed(int sampling_time_s, float &wind_speed_ms, float &wind_speed_kmh) {
    sensor_count_ = 0;
    delay(1000*sampling_time_s);

    wind_speed_ms = (1.492 * sensor_count_) / (float)sampling_time_s;
    wind_speed_kmh = wind_speed_ms * 3.6;
}