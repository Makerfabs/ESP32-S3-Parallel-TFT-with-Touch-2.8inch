#include <Wire.h>
#include <bb_captouch.h>

#define TOUCH_SDA 39
#define TOUCH_SCL 38
#define TOUCH_INT 40
#define TOUCH_RST 1

int16_t touch_last_x = 0;
int16_t touch_last_y = 0;

BBCapTouch bbct;

void touch_init()
{
  bbct.init(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);
}

bool touch_touched()
{
  TOUCHINFO ti;
  if (bbct.getSamples(&ti))
  {
    // *x = ti.x[0];
    // *y = ti.y[0];

    touch_last_x = ti.y[0];
    touch_last_y = map(ti.x[0], 240, 0, 0, 240);

    return true;
  }
  else
    return false;
}
