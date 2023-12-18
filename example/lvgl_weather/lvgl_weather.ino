#include <lvgl.h>
#include <ui.h>
#include "tft_config.h"
#include "touch.h"
#include <DHT11.h>

#define SCREEN_W 320
#define SCREEN_H 240

#define DHT11_PIN 19

LGFX lcd;
DHT11 dht11(DHT11_PIN);

int Fahrenheit = 0;

/*Don't forget to set Sketchbook location in File/Preferencesto the path of your UI project (the parent foder of this INO file)*/

/*Change to your screen resolution*/
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

void setup()
{
    Serial.begin(115200); /* prepare for possible serial debug */

    pinMode(TFT_BLK, OUTPUT);
    digitalWrite(TFT_BLK, HIGH);

    // Init touch device
    touch_init();

    // Init Display
    lcd.init();
    lcd.setRotation(1);
    lcd.fillScreen(TFT_WHITE);

    lv_init();

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    ui_init();

    Serial.println("Setup done");

    xTaskCreatePinnedToCore(Task_TFT, "Task_TFT", 10240, NULL, 0, NULL, 0);
    xTaskCreatePinnedToCore(Task_Sensor, "Task_Sensor", 2048, NULL, 1, NULL, 0);
}
void loop()
{
}

void Task_TFT(void *pvParameters)
{
    while (1)
    {
        lv_timer_handler(); /* let the GUI do its work */
        delay(5);
    }
}

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    lcd.pushImageDMA(area->x1, area->y1, w, h, (lgfx::rgb565_t *)&color_p->full);

    lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{

    if (touch_touched())
    {
        data->state = LV_INDEV_STATE_PR;

        /*Set the coordinates*/
        data->point.x = touch_last_x;
        data->point.y = touch_last_y;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }
}

void Task_Sensor(void *pvParameters)
{
    while (1)
    {

        int temperature = dht11.readTemperature();
        vTaskDelay(50);

        int humidity = dht11.readHumidity();

        lv_arc_set_value(ui_Arc1, temperature);

        if (temperature != DHT11::ERROR_CHECKSUM && temperature != DHT11::ERROR_TIMEOUT &&
            humidity != DHT11::ERROR_CHECKSUM && humidity != DHT11::ERROR_TIMEOUT)
        {

            String temp_str = "Temp: ";

            if (Fahrenheit == 0)
            {
                temp_str = temp_str + temperature + " C";

                Serial.print("Temperature: ");
                Serial.print(temperature);
                Serial.println(" °C");
            }
            else
            {
                temperature = temperature * 1.8 + 32;
                temp_str = temp_str + temperature + " F";

                Serial.print("Temperature: ");
                Serial.print(temperature);
                Serial.println(" °F");
            }

            lv_label_set_text(ui_Label1, temp_str.c_str());

            String humi_str = "Humi: ";
            humi_str = humi_str + humidity + " %";
            lv_label_set_text(ui_Label2, humi_str.c_str());
            lv_arc_set_value(ui_Arc2, humidity);

            Serial.print("Humidity: ");
            Serial.print(humidity);
            Serial.println(" %");
        }
        else
        {
            if (temperature == DHT11::ERROR_TIMEOUT || temperature == DHT11::ERROR_CHECKSUM)
            {
                Serial.print("Temperature Reading Error: ");
                Serial.println(DHT11::getErrorString(temperature));
            }
            if (humidity == DHT11::ERROR_TIMEOUT || humidity == DHT11::ERROR_CHECKSUM)
            {
                Serial.print("Humidity Reading Error: ");
                Serial.println(DHT11::getErrorString(humidity));
            }
        }

        vTaskDelay(2000);
    }
}
