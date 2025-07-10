#include <Arduino.h>
#include "setup.h"
#include "LCD/font5x8.h"

#define STRBUF_SIZE 60
char global_buf[STRBUF_SIZE];
uint32_t global_int_buf;
float global_float_buf;

/** RAM-safe state storage, of if the data has already been logged to SD card.
 * Specs:
 * - First 5 bits: for storing the hour (1-24).
 * - last bit: 1 when has been logged, 0 otherwise.
 * - 6th and 7th bits: reserved, no use for now.
 *
 * During each iteration of our `loop` function, we will check if we have logged
 * something in the current hour. Otherwise, log and set the last bit to 1.
 */
uint8_t is_logged_stat;

bool is_logged(uint8_t at_hour)
{
  if (at_hour != is_logged_stat >> 3)
  {
    return false;
  };
  return (is_logged_stat & 1) == 1;
}

void set_logged(uint8_t at_hour)
{
  is_logged_stat = (at_hour << 3) | 1;
  return;
}

void log_string(char *line)
{
  SD.begin();
  File file = setup_open_file();
  file.println(line);
  file.close();
  SD.end(); // if not called, card will burn to oblivion
  // this is fine, since this function should only be called once in a while.
}

inline void oled_off()
{
  oled.writeCommand(SSD1306_CMD_DISPLAYOFF);
}

inline bool hour_in_range(uint8_t val, uint8_t start, uint8_t end)
{
  if (val >= start && val <= end)
  {
    return true;
  }
  return false;
}

inline uint32_t timestamp()
{
  return rtc_clock.now().unixtime() - UTC_SHIFT * SECONDS_IN_AN_HOUR;
}

inline void set_peristaltic_pump(byte status)
{
  digitalWrite(PIN_PERISTALTIC_PUMP, status);
}

inline void set_aerator_pump(byte status)
{
  digitalWrite(PIN_AERATOR, status);
}

/** Write data to the global buf.
 * Warning: no buffer overflow check is done! This is to save our already crazy
 * RAM and flash due to the amount of sensors in this project.
 * At most we're prolly only gonna reach 50ish characters or so.
 */
void write_data_to_buf()
{
  size_t str_offset;

  // CSV column order:
  // time,water_temp,ambient_temp,ambient_humidity,eco2,light_intensity,turbidity
  global_int_buf = timestamp();
  itoa(global_int_buf, global_buf, 10);
  str_offset = strlen(global_buf);

  // pad with , and nul
  *(global_buf + str_offset++) = ',';
  *(global_buf + str_offset++) = '\0';

  thermometer.requestTemperatures();
  while (!thermometer.isConversionComplete())
    delay(100);
  global_int_buf = thermometer.getTempC();
  itoa(global_int_buf, global_buf + str_offset, 10);
  str_offset = strlen(global_buf);

  // pad with , and nul
  *(global_buf + str_offset++) = ',';
  *(global_buf + str_offset++) = '\0';

  global_int_buf = temp_humid.GetTemperature();
  itoa(global_int_buf, global_buf + str_offset, 10);
  str_offset = strlen(global_buf);

  // pad with , and nul
  *(global_buf + str_offset++) = ',';
  *(global_buf + str_offset++) = '\0';

  global_int_buf = temp_humid.GetHumidity();
  itoa(global_int_buf, global_buf + str_offset, 10);
  str_offset = strlen(global_buf);

  // pad with , and nul
  *(global_buf + str_offset++) = ',';
  *(global_buf + str_offset++) = '\0';

  eCO2.measure(true);
  global_int_buf = eCO2.geteCO2();
  itoa(global_int_buf, global_buf + str_offset, 10);
  str_offset = strlen(global_buf);

  // pad with , and nul
  *(global_buf + str_offset++) = ',';
  *(global_buf + str_offset++) = '\0';

  while (!light_intensity.measurementReady())
    delay(100);
  global_int_buf = light_intensity.readLightLevel();
  itoa(global_int_buf, global_buf + str_offset, 10);
  str_offset = strlen(global_buf);

  // pad with , and nul
  *(global_buf + str_offset++) = ',';
  *(global_buf + str_offset++) = '\0';

  global_float_buf = light_intensity.measurementReady();
  dtostrf(global_float_buf, 0, 2, global_buf + str_offset);
  str_offset = strlen(global_buf);

  // pad with , and nul
  *(global_buf + str_offset++) = ',';
  *(global_buf + str_offset++) = '\0';

  // haha
  global_int_buf = analogRead(PIN_TURBIDITY);
  itoa(global_int_buf, global_buf + str_offset, 10);
  str_offset = strlen(global_buf);

  // pad with , and nul
  *(global_buf + str_offset++) = ',';
  *(global_buf + str_offset++) = '\0';
}

// real stuff going on below!
void setup()
{
  setup_pin_mode();
  bool ok = true;
  Serial.begin(9600);
  Wire.begin();
  ok = ok && eCO2.begin();
  ok = ok && eCO2.available();
  eCO2.setMode(ENS160_OPMODE_STD);
  ok = ok && thermometer.begin();
  ok = ok && light_intensity.begin();
  ok = ok && temp_humid.begin();
  ok = ok && SD.begin(SD_CHIP_SELECT_PIN);
  oled.init();

  is_logged_stat = rtc_clock.now().hour() << 3;

#if DEBUG
  if (!ok)
  {
    Serial.println(F("Something went wrong when initializing sensors!"));
  }
#else
  Serial.println(ok);
#endif
}

void oled_print_clock(uint8_t hour, uint8_t min, uint8_t sec)
{
  oled.drawChar(0, OLED_LINE_1, ' ');
  oled.drawString(0, OLED_LINE_1, F("clck "));
  itoa(hour, global_buf, 10);

  oled.drawString(35, OLED_LINE_1, global_buf);

  oled.drawString(46, OLED_LINE_1, F(":"));
  itoa(min, global_buf, 10);

  oled.drawString(52, OLED_LINE_1, global_buf);

  oled.drawString(46, OLED_LINE_1, F(":"));
  itoa(sec, global_buf, 10);
}

void oled_show_collecting_data() {
  oled.drawString(0, OLED_LINE_2, F("logging.."));
}

void oled_done_collecting_data() {
  oled.drawString(56, OLED_LINE_2, F("ok"));
}

void loop()
{
  DateTime now = rtc_clock.now();

  uint8_t hour = now.hour();
  uint8_t min = now.minute();
  uint8_t sec = now.second();

  oled.clearDisplay();
  oled_print_clock(hour, min, sec);

  // we turn off the aerator if it's night..
  if (hour_in_range(hour, NIGHT_1_START, NIGHT_1_END) || hour_in_range(hour, NIGHT_2_START, NIGHT_2_END))
  {
    set_aerator_pump(LOW);
    set_peristaltic_pump(HIGH);
  }

  // a simple else would have worked, but, safety?
  if (hour_in_range(hour, DAY_START, DAY_END))
  {
    set_aerator_pump(HIGH);
    set_peristaltic_pump(HIGH);
  }

  // change when last hour differs
  // also, do not write `now << 3` since the last bit of is_logged_stat may be
  // set to 1.
  if ((is_logged_stat >> 3) != hour)
  {
    is_logged_stat = hour << 3;
  }

  if (hour % LOG_PERIOD == 0)
  {
    if (!is_logged(hour))
    {
      // turn off sensors to prevent voltage drop! this is so that the analog
      // sensor (im looking at you, turbidity sensor) will stay accurate.
      set_aerator_pump(LOW);
      set_peristaltic_pump(LOW);

      oled_show_collecting_data();

      // log stuff
      write_data_to_buf();
      log_string(global_buf);
      set_logged(hour);

      // turn our pumps back on
      set_aerator_pump(HIGH);
      set_peristaltic_pump(HIGH);

      oled_done_collecting_data();
      delay(1000);
    }
  }
  delay(2000);
}