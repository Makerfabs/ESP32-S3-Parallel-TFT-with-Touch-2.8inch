#include "tft_config.h"

LGFX lcd;

#define COLOR_NUM 7

int color_list[COLOR_NUM] =
    {
        TFT_BLACK,  /*   0,   0,   0 */
        TFT_RED,    /* 255,   0,   0 */
        TFT_GREEN,  /*   0, 255,   0 */
        TFT_BLUE,   /*   0,   0, 255 */
        TFT_YELLOW, /* 255, 255,   0 */
        TFT_PURPLE, /* 128,   0, 128 */
        TFT_WHITE}; /* 255, 255, 255 */

char color_name[COLOR_NUM][20] =
    {
        "TFT_BLACK", "TFT_RED", "TFT_GREEN", "TFT_BLUE",
        "TFT_YELLOW", "TFT_PURPLE", "TFT_WHITE"};

void setup()
{
    pinMode(TFT_BLK, OUTPUT);
    digitalWrite(TFT_BLK, HIGH);
    pinMode(TOUCH_RST, OUTPUT);
    digitalWrite(TOUCH_RST, LOW);
    delay(500);
    digitalWrite(TOUCH_RST, HIGH);
    delay(500);

    Serial.begin(115200);

    lcd.init();
    lcd.fillScreen(TFT_WHITE);

    for (int i = 0; i < COLOR_NUM; i++)
    {
        lcd.setTextColor(color_list[i]);
        lcd.setTextSize(4);
        lcd.setCursor(0, i * 32);
        lcd.print(color_name[i]);
    }
}

void loop()
{
}
