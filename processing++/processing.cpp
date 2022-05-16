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

  bool shouldStroke = true;
  bool shouldFill = true;

  ShapeAttributesMode shapeModeEllipse = CORNER;
  ShapeAttributesMode shapeModeRect = CORNER;  

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
} // namespace processing

int width() { return processing::width; }
int height() { return processing::height; }

void size(int w, int h)
{
  processing::width = w;
  processing::height = h;
}

void ellipseMode(ShapeAttributesMode mode) {
  processing::shapeModeEllipse = mode;
}

void rectMode(ShapeAttributesMode mode) {
  processing::shapeModeRect = mode;
}

void point(int x, int y, Color c)
{
  uint32_t ix = y * processing::width + x;
  processing::imageBuffer[ix] = MFB_RGB(c.r, c.g, c.b);
}

void rect(int a, int b, int c, int d)
{
  int x = a, y = b, w = c, h = d;
  switch (processing::shapeModeRect) {
    case CORNER:
      x = a;
      y = b;
      w = c;
      h = d;
      break;
    case CORNERS:
      x = a;
      y = b;
      w = c - a;
      h = d - b;
      break;
  }
  if (processing::shouldFill)
  {
    // For pixel-alignment in fills, use integer coordinates
    BLRectI rect{x, y, w, h};
    processing::ctx.fillGeometry(BLGeometryType::BL_GEOMETRY_TYPE_RECTI, &rect);
  }

  if (processing::shouldStroke)
  {
    // For pixel-alignment in strokes, use floating-point coordinates and start & end at pixel centers (i.e at x + .5, y + .5)
    BLRect rect{x + 0.5, y + 0.5, static_cast<double>(w), static_cast<double>(h)};
    processing::ctx.strokeGeometry(BLGeometryType::BL_GEOMETRY_TYPE_RECTD, &rect);
  }
}

void background(uint8_t r, uint8_t g, uint8_t b)
{
  for (auto ix = 0; ix < processing::width * processing::height; ++ix)
    processing::imageBuffer[ix] = MFB_RGB(r, g, b);
}

void fill(uint8_t gray)
{
  fill(gray, gray, gray);
}

void fill(uint8_t r, uint8_t g, uint8_t b)
{
  processing::shouldFill = true;
  processing::ctx.setFillStyle(BLRgba32{r, g, b, 255});
}

void noFill()
{
  processing::shouldFill = false;
}

void noStroke()
{
  processing::shouldStroke = false;
}

void stroke(uint8_t r, uint8_t g, uint8_t b)
{
  processing::shouldStroke = true;
  processing::ctx.setStrokeStyle(BLRgba32{r, g, b, 255});
}
