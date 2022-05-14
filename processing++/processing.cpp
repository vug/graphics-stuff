#include "processing.h"

namespace processing
{
  struct mfb_window *window;
  int width = 200;
  int height = 200;
  uint32_t *buffer;
  BLContext ctx;
  BLImage img;
  uint32_t *imageBuffer;

  void initImageBuffer()
  {
    BLImageData imgData;
    processing::img.getData(&imgData);
    imageBuffer = static_cast<uint32_t *>(imgData.pixelData);
  }

  bool waitSync()
  {
    return mfb_wait_sync(window);
  }

  mfb_update_state updateWindow()
  {
    return mfb_update_ex(window, buffer, width, height);
  }

  void resize(struct mfb_window *win, int w, int h)
  {
    (void)win;
    width = w;
    height = h;
    buffer = static_cast<uint32_t *>(realloc(buffer, width * height * sizeof(uint32_t)));
  }
}

int width() { return processing::width; }
int height() { return processing::height; }

void size(int w, int h)
{
  processing::width = w;
  processing::height = h;
}

void point(int x, int y, Color c)
{
  uint32_t ix = y * processing::width + x;
  processing::imageBuffer[ix] = MFB_RGB(c.r, c.g, c.b);
}

void background(int8_t r, int8_t g, int8_t b)
{
  for (auto ix = 0; ix < processing::width * processing::height; ++ix)
    processing::imageBuffer[ix] = MFB_RGB(r, g, b);
}
