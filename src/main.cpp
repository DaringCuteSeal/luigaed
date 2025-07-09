#include <Arduino.h>
#include "setup.h"

#define STRBUF_SIZE 51
char global_buf[STRBUF_SIZE];

void setup()
{
  setup_rtc();
  setup_pin_mode();
  bool ok = true;
  Serial.begin(9600);
  Wire.begin();
  ok = ok && eCO2.begin();
  ok = ok && thermometer.begin();
  ok = ok && light_intensity.begin();
  ok = ok && temp_humid.begin();
  oled.init();

#if DEBUG
  if (!ok)
  {
    Serial.println(F("Something went wrong when initializing sensors!"));
  }
#endif
}

void loop()
{
  delay(5000);
  digitalWrite(PIN_AERATOR, HIGH);
  digitalWrite(PIN_PERISTALTIC_PUMP, HIGH);
  Serial.println(FreeRam());

  oled.drawString(0, 0, "it is ");

  auto now = rtc_clock.now().hour();
  Serial.println(rtc_clock.now().unixtime());
  itoa(now, global_buf, 10);

  oled.drawString(35, 0, global_buf);

  oled.drawString(46, 0, ":");
  now = rtc_clock.now().minute();
  itoa(now, global_buf, 10);

  oled.drawString(52, 0, global_buf);

  oled.drawString(46, 0, ":");
  itoa(now, global_buf, 10);

  Serial.println(FreeRam());
  delay(5000);
  digitalWrite(PIN_PERISTALTIC_PUMP, LOW);
  digitalWrite(PIN_AERATOR, LOW);
}
