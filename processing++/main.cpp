#include "processing.h"

#include <cmath>

void setup();
void draw();

int main()
{
  setup();

  // struct mfb_timer *timer = mfb_timer_create();

  do
  {
    draw();
    memcpy(processing::buffer, processing::imageBuffer, processing::width * processing::height * sizeof(uint32_t));

    if (processing::updateWindow() != STATE_OK)
    {
      processing::window = 0x0;
      break;
    }
  } while (processing::waitSync());

  processing::ctx.end();
  return 0;
}