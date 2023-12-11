#include <bb_captouch.h>
#include "tft_config.h"

// These defines are for a low cost ESP32 LCD board with the GT911 touch controller
#define TOUCH_SDA 39
#define TOUCH_SCL 38
#define TOUCH_INT 40
#define TOUCH_RST 1

BBCapTouch bbct;
LGFX lcd;

void setup()
{
  pinMode(TFT_BLK, OUTPUT);
  digitalWrite(TFT_BLK, HIGH);

  Serial.begin(115200);

  lcd.init();
  lcd.fillScreen(TFT_WHITE);

  bbct.init(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);
}

void loop()
{
  uint16_t x, y;
  TOUCHINFO ti;

  if (bbct.getSamples(&ti))
  {
    x = ti.x[0];
    y = ti.y[0];
    Serial.print(x);
    Serial.print(",");
    Serial.println(y);

    lcd.fillRect(x - 2, y - 2, 4, 4, TFT_RED);
  }
}