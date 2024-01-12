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

// uint32_t glob_int_counter = 0;
// uint32_t glob_int_last = 0;
// bool glob_int_last_val = 0;


// void pulseCounter() {
//     const uint32_t now = micros();
//     if ((now - glob_int_last) >= INT_FILTER_US) {
//         glob_int_counter++;
//         glob_int_last = now;
//     }
// }

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
    readWindDirection(data->wind_dir, data->wind_heading);
    readWindSpeed(10, data->wind_speed_ms, data->wind_speed_kmh);
}

void Anemometer::readWindDirection(String &dir_name, uint16_t &dir_heading) {
    int val = analogRead(_dirPin);
    
    dir_name = "undefined";
    dir_heading = -1;

    // mapping based on a 10kOhm resistor 
    if (around(val, 2950, DIR_THRESHOLD)) {
        dir_name = "N";
        dir_heading = 0;
    }
    else if (around(val, 1650, DIR_THRESHOLD)) {
        dir_name = "NE";
        dir_heading = 45;
    }
    else if (around(val, 175, DIR_THRESHOLD)) {
        dir_name = "E";
        dir_heading = 90;
    }
    else if (around(val, 550, DIR_THRESHOLD)) {
        dir_name = "SE";
        dir_heading = 135;
    }
    else if (around(val, 950, DIR_THRESHOLD)) {
        dir_name = "S";
        dir_heading = 180;
    }
    else if (around(val, 2300, DIR_THRESHOLD)) {
        dir_name = "SW";
        dir_heading = 225;
    }
    else if (around(val, 3850, DIR_THRESHOLD)) {
        dir_name = "W";
        dir_heading = 270;
    }
    else if (around(val, 3450, DIR_THRESHOLD)) {
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