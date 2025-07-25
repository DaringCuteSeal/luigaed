#include "setup.h"

AnalogKeypad keypad(PIN_KEYPAD);
OneWire onewire(PIN_THERMOMETER);
DS18B20_INT thermometer(&onewire);

ScioSense_ENS160 eCO2(ADDR_eCO2);
ssd1306_128x64_i2c oled;
RTClib rtc_clock;
DS3231 rtc_hw;
File log_file;
BH1750 light_intensity;
AHT_Sensor_Class temp_humid;

void setup_pin_mode()
{
    pinMode(PIN_AERATOR, OUTPUT);
    pinMode(PIN_PERISTALTIC_PUMP, OUTPUT);
    pinMode(PIN_THERMOMETER, INPUT);
    pinMode(PIN_BATTERY, INPUT);
    pinMode(PIN_TURBIDITY, INPUT);
    pinMode(SD_CHIP_SELECT_PIN, OUTPUT);
}

File setup_open_file()
{
    return SD.open(LOG_FILENAME, FILE_WRITE);
}