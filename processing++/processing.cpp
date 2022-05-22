#include "processing.h"

#include <cassert>

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
  ShapeAttributesCap shapeCap = ROUND;
  ShapeVertexBeginMode shapeVertexBeginMode = POLYGON;
  ShapeVertexEndMode shapeVertexEndMode = NONE;
  std::vector<BLPointI> shapeVertices = {};

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
    strokeCap(ROUND);
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

  BLRgba32 getStrokeColor()
  {
    BLVarCore var;
    BLRgba32 col;
    processing::ctx.getStrokeStyle(var);
    blVarToRgba32(&var, &col.value);
    return col;
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

void strokeCap(ShapeAttributesCap cap)
{
  switch (cap)
  {
  case ShapeAttributesCap::ROUND:
    processing::ctx.setStrokeCaps(BL_STROKE_CAP_ROUND);
    break;
  case ShapeAttributesCap::SQUARE:
    processing::ctx.setStrokeCaps(BL_STROKE_CAP_BUTT);
    break;
  case ShapeAttributesCap::PROJECT:
    processing::ctx.setStrokeCaps(BL_STROKE_CAP_SQUARE);
    break;
  }
}

void strokeWeight(double w)
{
  processing::ctx.setStrokeWidth(w);
}

void beginShape(ShapeVertexBeginMode mode)
{
  processing::shapeVertices.clear();
  processing::shapeVertexBeginMode = mode;
}

void vertex(int x, int y)
{
  processing::shapeVertices.emplace_back(x, y);
}

void endShape(ShapeVertexEndMode mode)
{
  BLPath path;
  const auto &verts = processing::shapeVertices;
  switch (processing::shapeVertexBeginMode)
  {
  case ShapeVertexBeginMode::POLYGON:
  {
    for (size_t ix = 0; ix < verts.size(); ++ix)
    {
      const auto &p = verts[ix];
      if (ix == 0)
        path.moveTo(p.x, p.y);
      else
        path.lineTo(p.x, p.y);
    }
    if (mode == ShapeVertexEndMode::CLOSE)
      path.close();
  }
  break;
  case ShapeVertexBeginMode::POINTS:
  {
    for (const auto &p : verts)
      point(p.x, p.y);
  }
  break;
  case ShapeVertexBeginMode::LINES:
  {
    assert(verts.size() % 2 == 0);
    for (size_t ix = 0; ix < verts.size(); ix += 2)
    {
      const auto &p1 = verts[ix];
      const auto &p2 = verts[ix + 1];
      const BLLine line{static_cast<double>(p1.x), static_cast<double>(p1.y), static_cast<double>(p2.x), static_cast<double>(p2.y)};
      path.addLine(line);
    }
  }
  break;
  case ShapeVertexBeginMode::TRIANGLES:
  {
    assert(verts.size() % 3 == 0);
    for (size_t ix = 0; ix < verts.size(); ix += 3)
    {
      const auto &p1 = verts[ix];
      const auto &p2 = verts[ix + 1];
      const auto &p3 = verts[ix + 2];
      const BLTriangle triangle{
          static_cast<double>(p1.x), static_cast<double>(p1.y),
          static_cast<double>(p2.x), static_cast<double>(p2.y),
          static_cast<double>(p3.x), static_cast<double>(p3.y)};
      path.addTriangle(triangle);
    }
  }
  break;
  case ShapeVertexBeginMode::TRIANGLE_STRIP:
  {
    assert(verts.size() >= 3);
    size_t ix = 2;
    do
    {
      const BLPointI &p1 = verts[ix - 2];
      const BLPointI &p2 = verts[ix - 1];
      const BLPointI &p3 = verts[ix];
      const BLTriangle triangle{
          static_cast<double>(p1.x), static_cast<double>(p1.y),
          static_cast<double>(p2.x), static_cast<double>(p2.y),
          static_cast<double>(p3.x), static_cast<double>(p3.y)};
      path.addTriangle(triangle);
      ++ix;
    } while (ix < verts.size());
  }
  break;
  case ShapeVertexBeginMode::TRIANGLE_FAN:
  {
    assert(verts.size() >= 3);
    size_t ix = 2;
    const BLPointI &p1 = verts[0];
    do
    {
      const BLPointI &p2 = verts[ix - 1];
      const BLPointI &p3 = verts[ix];
      const BLTriangle triangle{
          static_cast<double>(p1.x), static_cast<double>(p1.y),
          static_cast<double>(p2.x), static_cast<double>(p2.y),
          static_cast<double>(p3.x), static_cast<double>(p3.y)};
      path.addTriangle(triangle);
      ++ix;
    } while (ix < verts.size());
  }
  break;
  case ShapeVertexBeginMode::QUADS:
  {
    assert(verts.size() % 4 == 0);
    for (size_t ix = 0; ix < verts.size(); ix += 4)
    {
      const auto &p1 = verts[ix];
      const auto &p2 = verts[ix + 1];
      const auto &p3 = verts[ix + 2];
      const auto &p4 = verts[ix + 3];
      const std::vector<BLPointI> quad = {p1, p2, p3, p4};
      path.addPolygon(quad.data(), quad.size());
    }
  }
  break;
  case ShapeVertexBeginMode::QUAD_STRIP:
  {
    assert(verts.size() >= 4);
    size_t ix = 3;
    do
    {
      const BLPointI &p1 = verts[ix - 3];
      const BLPointI &p2 = verts[ix - 2];
      const BLPointI &p3 = verts[ix];
      const BLPointI &p4 = verts[ix - 1];
      const std::vector<BLPointI> quad = {p1, p2, p3, p4};
      path.addPolygon(quad.data(), quad.size());
      ix += 2;
    } while (ix < verts.size());
  }
  break;
  }

  if (processing::shouldFill)
    processing::ctx.fillPath(path);

  if (processing::shouldStroke)
    processing::ctx.strokePath(path);
}

void point(int x, int y)
{
  const auto &c = processing::getStrokeColor();
  uint32_t ix = y * processing::width + x;
  processing::imageBuffer[ix] = MFB_RGB(c.r(), c.g(), c.b());
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

void line(int x1, int y1, int x2, int y2)
{
  BLLine line{static_cast<double>(x1), static_cast<double>(y1), static_cast<double>(x2), static_cast<double>(y2)};
  processing::ctx.strokeGeometry(BLGeometryType::BL_GEOMETRY_TYPE_LINE, &line);
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
