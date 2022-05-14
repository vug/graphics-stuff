#include "processing.h"

Processing processing{};

bool Processing::waitSync()
{
  return mfb_wait_sync(window);
}

mfb_update_state Processing::updateWindow()
{
  return mfb_update_ex(window, buffer, width, height);
}

void Processing::size(int w, int h)
{
  width = w;
  height = h;
  window = mfb_open_ex("MiniFB Test", width, height, WF_BORDERLESS || WF_RESIZABLE);
  buffer = new uint32_t[width * height * sizeof(uint32_t)];

  mfb_set_viewport(window, 0, 0, width, height);
}

void size(int w, int h)
{
  processing.size(w, h);
}

void Processing::point(int x, int y, Color c)
{
  uint32_t ix = y * width + x;
  buffer[ix] = MFB_RGB(c.r, c.g, c.b);
}

void point(int x, int y, Color c)
{
  processing.point(x, y, c);
}
