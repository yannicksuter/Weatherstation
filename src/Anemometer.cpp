#include "Anemometer.h"

#include <Arduino.h>

#define DIR_THRESHOLD 50

bool between(int val, int lower_limit, int upper_limit) {
    return (val > lower_limit && val < upper_limit);
}

bool around(int val, int ref, int threshold) {
    return between(val, ref-threshold, ref+threshold);
}

void Anemometer::setup(int dirPin) {
    _dirPin = dirPin;

    analogSetPinAttenuation(_dirPin, ADC_11db);
}

void Anemometer::readWindDirection() {

    int val = analogRead(_dirPin);
    
    String dir_name = "undefined";
    int dir_heading = -1;

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

    static char strBuf[250];
    sprintf(strBuf, "value = %d, dir=%s, heading=%d", val, dir_name.c_str(), dir_heading);
    Serial.println(strBuf);
}
