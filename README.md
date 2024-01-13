This repository contains the source code for a weather monitoring system implemented on an ESP32 microcontroller using Arduino. The system collects data from various sensors, including a BME280 sensor for temperature, pressure, and humidity measurements, an INA3221 sensor for power monitoring, an anemometer for wind speed and direction, and a rain gauge for measuring rainfall.

#Todo
- reset s35770 internal counter on first bootup (just to be sure it always starts at 0)
- adding power monitoring to HA metrics
- adding wind direction and speed monitoring to HA metrics
- refactor HAInterface to make metrics-channel creation simpler (less redundant code..), maybe with a registry
- attach solar panels
- casing, CAD parts and outlet
- maybe 2 solarpanel sections (east-west) to double recharge capacity

#New PCB revision
- i2c multiplexing, evaluate which multiplexer to use
- have a second s35770 counter for windspeed and raingauge
- onboard buck/boost
