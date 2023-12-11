/*
使用 1.0.1  版本的库 bb_captouch 在文件夹： C:\Users\maker\Documents\Arduino\libraries\bb_captouch
使用 2.0.0  版本的库 Wire 在文件夹： C:\Users\maker\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.11\libraries\Wire
使用 1.1.9  版本的库 LovyanGFX 在文件夹： C:\Users\maker\Documents\Arduino\libraries\LovyanGFX
使用 2.0.0  版本的库 FS 在文件夹： C:\Users\maker\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.11\libraries\FS
使用 2.0.0  版本的库 SD_MMC 在文件夹： C:\Users\maker\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.11\libraries\SD_MMC
使用 1.2.8  版本的库 JPEGDEC 在文件夹： C:\Users\maker\Documents\Arduino\libraries\JPEGDEC

*/

#include <bb_captouch.h>
#include "tft_config.h"
#include "FS.h"
#include "SD_MMC.h"
#include "JpegFunc.h"

#define PIN_SD_CMD 2
#define PIN_SD_CLK 42
#define PIN_SD_D0 41

#define TOUCH_SDA 39
#define TOUCH_SCL 38
#define TOUCH_INT 40
#define TOUCH_RST 1

#define SCREEN_W 240
#define SCREEN_H 320

#define JPEG_FILENAME_LOGO "/logo.jpg"

BBCapTouch bbct;
LGFX lcd;

int draw_type = 0;

static int colors[] = {
    TFT_RED,
    TFT_GREEN,
    TFT_BLUE,
    TFT_WHITE,
    TFT_BLACK,
};

void setup()
{
  pinMode(TFT_BLK, OUTPUT);
  digitalWrite(TFT_BLK, HIGH);

  Serial.begin(115200);

  lcd.init();
  lcd.setRotation(1);
  lcd.fillScreen(TFT_WHITE);
  lcd.setTextColor(TFT_BLACK);
  lcd.setTextSize(2);
  lcd.setCursor(0, 140);
  lcd.println(" MaTouch ESP32-S3");
  lcd.println(" Parallel TFT with Touch");
  lcd.println(" 2.8\" ST7789 V1.0");

  bbct.init(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);

  SD_MMC.setPins(PIN_SD_CLK, PIN_SD_CMD, PIN_SD_D0);
  if (!SD_MMC.begin("/sdcard", true, true))
  {

    while (1)
    {
      Serial.println("Card Mount Failed");
      delay(1000);
    }
  }

  jpegDraw(JPEG_FILENAME_LOGO, jpegDrawCallback, true /* useBigEndian */,
           0 /* x */, 0 /* y */, SCREEN_W /* widthLimit */, SCREEN_H /* heightLimit */);
  delay(2000);

  for (int i = 0; i < 5; i++)
  {
    lcd.fillScreen(colors[i]);
    delay(1000);
  }

  delay(1000);

  lcd.fillScreen(TFT_WHITE);
  lcd.setTextColor(TFT_BLACK);
  lcd.setTextSize(4);
  lcd.setCursor(0, 140);
  lcd.print("   Touch");
}

void loop()
{
  uint16_t x, y;

  if (get_touch(&x, &y))
  {

    Serial.print(x);
    Serial.print(",");
    Serial.println(y);

    lcd.fillRect(x - 3, y - 3, 6, 6, TFT_RED);
  }
}

int get_touch(uint16_t *x, uint16_t *y)
{
  TOUCHINFO ti;
  if (bbct.getSamples(&ti))
  {
    // *x = ti.x[0];
    // *y = ti.y[0];

    *x = ti.y[0];
    *y = map(ti.x[0], 240, 0, 0, 240);

    return 1;
  }
  else
    return 0;
}

static int jpegDrawCallback(JPEGDRAW *pDraw)
{
  lcd.pushImage(pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight, (lgfx::swap565_t *)pDraw->pPixels);

  //   lcd.pushImageDMA(pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight, (lgfx::swap565_t *)pDraw->pPixels);
  //   lcd.pushImageDMA(pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight, (lgfx::rgb565_t *)pDraw->pPixels);
  //   lcd.pushImageDMA(pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight, (lgfx::rgb888_t *)pDraw->pPixels);
  //   lcd.pushImage(pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight, (lgfx::swap565_t *)pDraw->pPixels);
  //   lcd.pushImage(pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight, (lgfx::rgb565_t *)pDraw->pPixels);
  //   lcd.pushImage(pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight, (lgfx::rgb888_t *)pDraw->pPixels);

  return 1;
}
