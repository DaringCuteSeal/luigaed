#pragma once

// Pinouts
#define PIN_KEYPAD PIN_A2
#define PIN_TURBIDITY A1
#define PIN_BATTERY A0
#define PIN_WATER_SENSOR_LIGHT A6
#define PIN_WATER_SENSOR_DARK A7
#define PIN_PERISTALTIC_PUMP 2
#define PIN_AERATOR 3
#define PIN_THERMOMETER 5
#define PIN_SD_CS 10

// I²C Components
#define ADDR_OLED 0x3C
#define ADDR_eCO2 0x53
#define ADDR_AHT21 0x38
#define ADDR_RTC 0x68
#define ADDR_RTC_EEPROM 0x57
#define ADDR_LIGHT_INTENSITY 0x23

// Specials
#define PIN_SENSORS_POWER

// Other stuff goes here...
#define LOG_FILENAME "log.csv"
#define LOG_PERIOD 1 // Log period: log if 24 mod x = 0  where x is the definition of LOG_PERIOD
#define UTC_SHIFT 7  // WIB (where it was deployed) (UTC+7)
#define SECONDS_IN_AN_HOUR 3600

// all below are in 24h
// aerator will turn off at night. this is used for that.
#define NIGHT_1_START 0
#define NIGHT_1_END 6
#define DAY_START 7
#define DAY_END 17
#define NIGHT_2_START 18
#define NIGHT_2_END 23