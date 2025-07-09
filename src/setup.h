#pragma once

#define DEBUG false

#include <Wire.h>
#include <BH1750.h>
#include "LCD/ssd1306_128x64_i2c.h"
#include <ScioSense_ENS160.h>
#include <DS18B20_INT.h>
#include <DS3231.h>
#include <SPI.h>
#include <SD.h>
#include <AnalogKeypad.h>
#include "AHT/AHT_Sensor/src/Thinary_AHT_Sensor.h"

#include "config.h"

extern AnalogKeypad keypad;
extern OneWire onewire;
extern DS18B20_INT thermometer;

extern ScioSense_ENS160 eCO2;
extern ssd1306_128x64_i2c oled;
extern RTClib rtc_clock;
extern DS3231 rtc_hw;
extern BH1750 light_intensity;
extern AHT_Sensor_Class temp_humid;

void setup_pin_mode();

File setup_open_file();