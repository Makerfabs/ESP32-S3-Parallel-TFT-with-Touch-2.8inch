// 使用 2.0.0  版本的库 FS 在文件夹： C:\Users\maker\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.11\libraries\FS
// 使用 2.0.0  版本的库 SD_MMC 在文件夹： C:\Users\maker\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.11\libraries\SD_MMC
// 使用 1.1.9  版本的库 LovyanGFX 在文件夹： C:\Users\maker\Documents\Arduino\libraries\LovyanGFX

#include <FS.h>
#include <SD_MMC.h>

#include "GifClass.h"
#include "tft_config.h"

#define AUTO_MODE 1

#define PIN_SD_CMD 2
#define PIN_SD_CLK 42
#define PIN_SD_D0 41

#define SCREEN_W 320
#define SCREEN_H 240

LGFX lcd;

#define GIF_NUM 5
String gif_list[GIF_NUM] =
    {
        "/d1.gif",
        "/d2.gif",
        "/d3.gif",
        "/d4.gif",
        "/d5.gif"};
int gif_bg[GIF_NUM] =
    {
        TFT_WHITE,
        TFT_BLACK,
        TFT_BLACK,
        TFT_BLACK,
        TFT_BLACK};

int gif_index = 0;
int fresh_flag = 1;

static GifClass gifClass;

void setup()
{
    pinMode(TFT_BLK, OUTPUT);
    digitalWrite(TFT_BLK, HIGH);

    Serial.begin(115200);

    lcd.init();
    lcd.setRotation(1);
    lcd.fillScreen(TFT_WHITE);

    SD_MMC.setPins(PIN_SD_CLK, PIN_SD_CMD, PIN_SD_D0);
    if (!SD_MMC.begin("/sdcard", true, true))
    {
        while (1)
        {
            Serial.println("Card Mount Failed");
            delay(1000);
        }
    }
}

void loop()
{

    while (1)
    {
        gif_index++;
        gif_index %= GIF_NUM;

        lcd.fillScreen(gif_bg[gif_index]);

        File gifFile = SD_MMC.open(gif_list[gif_index], "r");
        if (!gifFile || gifFile.isDirectory())
        {
            Serial.println(F("ERROR: open gifFile Failed!"));
        }
        else
        {
            // read GIF file header
            gd_GIF *gif = gifClass.gd_open_gif(&gifFile);
            if (!gif)
            {
                Serial.println(F("gd_open_gif() failed!"));
            }
            else
            {
                int buff_size = gif->width * gif->height;
                Serial.print("Buffer size:");
                Serial.println(buff_size);

                uint8_t *buf = (uint8_t *)malloc(buff_size);
                delay(10);

                if (!buf)
                {
                    Serial.println(F("buf malloc failed!"));
                }
                else
                {
                    int16_t x = (SCREEN_W - gif->width) / 2;
                    int16_t y = (SCREEN_H - gif->height) / 2;

                    Serial.println(F("GIF video start"));
                    int32_t t_fstart, t_delay = 0, t_real_delay, delay_until;
                    int32_t res = 1;

                    while (res > 0)
                    {
                        t_fstart = millis();
                        t_delay = gif->gce.delay * 10;
                        res = gifClass.gd_get_frame(gif, buf);
                        if (res < 0)
                        {
                            Serial.println(F("ERROR: gd_get_frame() failed!"));
                            break;
                        }
                        else if (res > 0)
                        {
                            bitmap_to_rgb332(buf, gif->palette->colors, buff_size);
                            lcd.pushImage(x, y, gif->width, gif->height, (lgfx::rgb332_t *)buf);

                            t_real_delay = t_delay - (millis() - t_fstart);

                            delay_until = millis() + t_real_delay;
                            while (millis() < delay_until)
                            {
                                vTaskDelay(5);

                                if (fresh_flag == 1)
                                {
                                    break;
                                }
                            }
                        }
                    }

                    gifClass.gd_close_gif(gif);
                }
                free(buf);
            }
        }
    }
}

void bitmap_to_rgb332(uint8_t *src, uint16_t *color, int count)
{
    for (int i = 0; i < count; i++)
    {
        uint16_t pixel = color[src[i]];
        src[i] = lcd.color16to8(pixel);
    }
}