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

  ShapeAttributesMode shapeModeEllipse = CENTER;
  ShapeAttributesMode shapeModeRect = CORNER;

  void initContext()
  {
    img = BLImage{width, height, BL_FORMAT_PRGB32};
    BLImageData imgData;
    img.getData(&imgData);
    imageBuffer = static_cast<uint32_t *>(imgData.pixelData);

    ctx = BLContext{img};
    ctx.clearAll();
    ctx.setRenderingQuality(BL_RENDERING_QUALITY_ANTIALIAS);

    ctx.setCompOp(BL_COMP_OP_SRC_OVER);
    ctx.setFillStyle(BLRgba32{255, 255, 255, 255});
    ctx.setStrokeStyle(BLRgba32{0, 0, 0, 255});
    ctx.setStrokeWidth(1);
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

    mfb_set_viewport(processing::window, 0, 0, processing::width, processing::height);
    initContext();
  }
} // namespace processing

int width() { return processing::width; }
int height() { return processing::height; }

void size(int w, int h)
{
  processing::width = w;
  processing::height = h;

  // TODO(vug): better API for resizability. See: https://processing.org/reference/setResizable_.html
  // By default it should be not resizable.
  // TODO(vug): second call to size should update existing window, not create a new one.
  processing::window = mfb_open_ex("MiniFB Test", processing::width, processing::height, WF_RESIZABLE);
  mfb_set_resize_callback(processing::window, [](struct mfb_window *window, int w, int h)
                          { processing::resize(window, w, h); });

  processing::buffer = new uint32_t[processing::width * processing::height * sizeof(uint32_t)];

  mfb_set_viewport(processing::window, 0, 0, processing::width, processing::height);
  processing::initContext();

  background(211, 211, 211);
}

void ellipseMode(ShapeAttributesMode mode)
{
  processing::shapeModeEllipse = mode;
}

void rectMode(ShapeAttributesMode mode)
{
  processing::shapeModeRect = mode;
}

void point(int x, int y, Color c)
{
  uint32_t ix = y * processing::width + x;
  processing::imageBuffer[ix] = MFB_RGB(c.r, c.g, c.b);
}

void circle(int x, int y, int r)
{
  ShapeAttributesMode tmp = processing::shapeModeEllipse;
  ellipseMode(CENTER);
  ellipse(x, y, r, r);
  ellipseMode(tmp);
}

void ellipse(int a, int b, int c, int d)
{
  int cx = a, cy = b, rx = c, ry = d;
  switch (processing::shapeModeEllipse)
  {
  case CORNER:
    cx = a + c / 2;
    cy = b + d / 2;
    rx = c / 2;
    ry = d / 2;
    break;
  case CORNERS:
    cx = (a + c) / 2;
    cy = (b + d) / 2;
    rx = (c - a) / 2;
    ry = (d - b) / 2;
    break;
  case CENTER:
    cx = a;
    cy = b;
    rx = c / 2;
    ry = d / 2;
    break;
  case RADIUS:
    cx = a;
    cy = b;
    rx = c;
    ry = d;
    break;
  }

  BLEllipse ellipse{static_cast<double>(cx), static_cast<double>(cy), static_cast<double>(rx), static_cast<double>(ry)};
  if (processing::shouldFill)
    processing::ctx.fillGeometry(BLGeometryType::BL_GEOMETRY_TYPE_ELLIPSE, &ellipse);

  if (processing::shouldStroke)
    processing::ctx.strokeGeometry(BLGeometryType::BL_GEOMETRY_TYPE_ELLIPSE, &ellipse);
}

void rect(int a, int b, int c, int d, int r)
{
  int x = a, y = b, w = c, h = d;
  switch (processing::shapeModeRect)
  {
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
  case CENTER:
    x = a - c / 2;
    y = b - d / 2;
    w = c;
    h = d;
    break;
  case RADIUS:
    x = a - c;
    y = b - d;
    w = 2 * c;
    h = 2 * d;
    break;
  }
  if (r > 0)
  {
    BLRoundRect roundRect{static_cast<double>(x), static_cast<double>(y), static_cast<double>(w), static_cast<double>(h), static_cast<double>(r), static_cast<double>(r)};
    if (processing::shouldFill)
      processing::ctx.fillGeometry(BLGeometryType::BL_GEOMETRY_TYPE_ROUND_RECT, &roundRect);

    if (processing::shouldStroke)
      processing::ctx.strokeGeometry(BLGeometryType::BL_GEOMETRY_TYPE_ROUND_RECT, &roundRect);
  }
  else
  {
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
}

void square(int x, int y, int s)
{
  ShapeAttributesMode tmp = processing::shapeModeRect;
  rectMode(CORNER);
  rect(x, y, s, s);
  rectMode(tmp);
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
