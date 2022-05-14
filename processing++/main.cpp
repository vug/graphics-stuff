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

  processing::img = BLImage{
      processing::width, processing::height, BL_FORMAT_PRGB32};
  processing::initImageBuffer();

  processing::ctx = BLContext{processing::img};
  processing::ctx.setRenderingQuality(BL_RENDERING_QUALITY_ANTIALIAS);
  processing::ctx.setCompOp(BL_COMP_OP_SRC_COPY);
  processing::ctx.fillAll();

  processing::ctx.setCompOp(BL_COMP_OP_SRC_OVER);
  processing::ctx.setFillStyle(BLRgba32{255, 255, 255, 255});
  processing::ctx.setStrokeStyle(BLRgba32{0, 0, 0, 255});
  processing::ctx.setStrokeWidth(1);
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