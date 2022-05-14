#include "processing.h"

#include <cmath>

void setup();
void draw();

int main()
{
  setup();

  processing::window = mfb_open_ex("MiniFB Test", processing::width, processing::height, WF_BORDERLESS || WF_RESIZABLE);
  processing::buffer = new uint32_t[processing::width * processing::height * sizeof(uint32_t)];
  mfb_set_viewport(processing::window, 0, 0, processing::width, processing::height);

  mfb_set_resize_callback(processing::window, [](struct mfb_window *window, int w, int h)
                          { processing::resize(window, w, h); });
  // struct mfb_timer *timer = mfb_timer_create();

  do
  {
    draw();

    if (processing::updateWindow() != STATE_OK)
    {
      processing::window = 0x0;
      break;
    }
  } while (processing::waitSync());

  return 0;
}