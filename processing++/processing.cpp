#include "processing.h"
#include <ColorSpace.h>

#include <cassert>

color::color(uint8_t gray)
    : value({gray, gray, gray, 255}) {}
color::color(int v1, int v2, int v3)
    : value(processing::getColorFromTriplet(v1, v2, v3)) {}
color::color(float v1, float v2, float v3)
    : value(processing::getColorFromTriplet(v1, v2, v3)) {}

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
  ShapeAttributesCapOrJoin shapeCap = ShapeAttributesCapOrJoin::ROUND;
  ShapeAttributesCapOrJoin shapeJoin = ShapeAttributesCapOrJoin::MITER;
  ShapeVertexBeginMode shapeVertexBeginMode = POLYGON;
  ShapeVertexEndMode shapeVertexEndMode = NONE;
  std::vector<BLPointI> shapeVertices = {};
  ColorMode colorModeSetting = ColorMode::RGB;
  std::array<float, 3> colorMaxSetting = {255.f, 255.f, 255.f};
  const std::array<float, 3> colorMaxRgb = {255.f, 255.f, 255.f};
  const std::array<float, 3> colorMaxHsb = {360.f, 1.f, 1.f};

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

  BLRgba32 getColorFromTripletNormalized(float v1, float v2, float v3)
  {
    switch (colorModeSetting)
    {
    case ColorMode::RGB:
      return BLRgba32{static_cast<uint32_t>(v1), static_cast<uint32_t>(v2), static_cast<uint32_t>(v3)};
    case ColorMode::HSB:
    {
      ColorSpace::Hsb hsb{static_cast<double>(v1), static_cast<double>(v2), static_cast<double>(v3)};
      ColorSpace::Rgb rgb;
      hsb.ToRgb(&rgb);
      return BLRgba32{static_cast<uint8_t>(rgb.r), static_cast<uint8_t>(rgb.g), static_cast<uint8_t>(rgb.b)};
    }
    default:
      assert(false); // Unknown color mode
      return BLRgba32{0xFFFFFFFF};
    }
  }

  BLRgba32 getColorFromTriplet(int v1, int v2, int v3)
  {
    return getColorFromTriplet(static_cast<float>(v1), static_cast<float>(v2), static_cast<float>(v3));
  }

  BLRgba32 getColorFromTriplet(float v1, float v2, float v3)
  {
    const auto &colorMax = processing::colorModeSetting == ColorMode::RGB ? colorMaxRgb : colorMaxHsb;
    const float n1 = v1 / colorMaxSetting[0] * colorMax[0];
    const float n2 = v2 / colorMaxSetting[1] * colorMax[1];
    const float n3 = v3 / colorMaxSetting[2] * colorMax[2];
    return getColorFromTripletNormalized(n1, n2, n3);
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

void strokeCap(ShapeAttributesCapOrJoin cap)
{
  switch (cap)
  {
  case ShapeAttributesCapOrJoin::ROUND:
    processing::ctx.setStrokeCaps(BL_STROKE_CAP_ROUND);
    break;
  case ShapeAttributesCapOrJoin::SQUARE:
    processing::ctx.setStrokeCaps(BL_STROKE_CAP_BUTT);
    break;
  case ShapeAttributesCapOrJoin::PROJECT:
    processing::ctx.setStrokeCaps(BL_STROKE_CAP_SQUARE);
    break;
  default:
    assert(false);
    break;
  }
}

void strokeJoin(ShapeAttributesCapOrJoin join)
{
  switch (join)
  {
  case ShapeAttributesCapOrJoin::ROUND:
    processing::ctx.setStrokeJoin(BL_STROKE_JOIN_ROUND);
    break;
  case ShapeAttributesCapOrJoin::MITER:
    processing::ctx.setStrokeJoin(BL_STROKE_JOIN_MITER_CLIP);
    break;
  case ShapeAttributesCapOrJoin::BEVEL:
    processing::ctx.setStrokeJoin(BL_STROKE_JOIN_BEVEL);
    break;
  default:
    assert(false);
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

void background(color c)
{
  const auto col = c.getValue();
  for (auto ix = 0; ix < processing::width * processing::height; ++ix)
    processing::imageBuffer[ix] = MFB_RGB(col.r(), col.g(), col.b());
}

void background(uint8_t v1, uint8_t v2, uint8_t v3)
{
  const auto c = processing::getColorFromTriplet(v1, v2, v3);
  for (auto ix = 0; ix < processing::width * processing::height; ++ix)
    processing::imageBuffer[ix] = MFB_RGB(c.r(), c.g(), c.b());
}

void colorMode(ColorMode mode, float max)
{
  processing::colorModeSetting = mode;
  processing::colorMaxSetting = {max, max, max};
}

void colorMode(ColorMode mode, float max1, float max2, float max3)
{
  processing::colorModeSetting = mode;
  processing::colorMaxSetting = {max1, max2, max3};
}

void fill(color c)
{
  processing::shouldFill = true;
  processing::ctx.setFillStyle(c.getValue());
}

void fill(uint8_t gray)
{
  fill(gray, gray, gray);
}

void fill(int v1, int v2, int v3)
{
  processing::shouldFill = true;
  const auto col = processing::getColorFromTriplet(v1, v2, v3);
  processing::ctx.setFillStyle(col);
}

void fill(float v1, float v2, float v3)
{
  processing::shouldFill = true;
  const auto col = processing::getColorFromTriplet(v1, v2, v3);
  processing::ctx.setFillStyle(col);
}

void noFill()
{
  processing::shouldFill = false;
}

void noStroke()
{
  processing::shouldStroke = false;
}

void stroke(color c)
{
  processing::shouldFill = true;
  processing::ctx.setStrokeStyle(c.getValue());
}

void stroke(uint8_t gray)
{
  stroke(gray, gray, gray);
}

void stroke(int v1, int v2, int v3)
{
  processing::shouldStroke = true;
  const auto col = processing::getColorFromTriplet(v1, v2, v3);
  processing::ctx.setStrokeStyle(col);
}

void stroke(float v1, float v2, float v3)
{
  processing::shouldStroke = true;
  const auto col = processing::getColorFromTriplet(v1, v2, v3);
  processing::ctx.setStrokeStyle(col);
}
