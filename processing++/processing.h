#pragma once
#include <MiniFB.h>
#define BL_STATIC
#include <blend2d.h>

#include <array>
#include <vector>

// Both Cap and Join has ROUND value. Since we are not using `enum class` but just `enum`
// as a hacky solution combined enumarations of Cap and Join.
enum ShapeAttributesCapOrJoin
{
  // Cap
  ROUND = 0,
  SQUARE = 1,
  PROJECT = 2,
  // Join
  // ROUND,
  MITER,
  BEVEL,
};

enum ShapeAttributesMode
{
  CORNER = 0,
  CORNERS = 1,
  CENTER = 2,
  RADIUS = 3,
};

enum ShapeVertexBeginMode
{
  POLYGON = 0,
  POINTS,
  LINES,
  TRIANGLES,
  TRIANGLE_FAN,
  TRIANGLE_STRIP,
  QUADS,
  QUAD_STRIP,
};

enum ShapeVertexEndMode
{
  NONE = 0,
  CLOSE = 1,
};

enum ColorMode
{
  RGB = 0,
  HSB,
};

class color
{
public:
  color() = default;
  color(uint8_t gray);
  color(int v1, int v2, int v3);
  color(float v1, float v2, float v3);

  const BLRgba32 getValue() const { return value; }

private:
  BLRgba32 value{0xFFFFFFFF};
};

namespace processing
{
  extern struct mfb_window *window;
  extern int width;
  extern int height;
  extern uint32_t *buffer;
  extern BLContext ctx;
  extern BLImage img;
  extern uint32_t *imageBuffer;

  extern bool shouldStroke;
  extern bool shouldFill;

  extern ShapeAttributesMode shapeModeEllipse;
  extern ShapeAttributesMode shapeModeRect;
  extern ShapeAttributesCapOrJoin shapeCap;
  extern ShapeAttributesCapOrJoin shapeJoin;
  extern ShapeVertexBeginMode shapeVertexBeginMode;
  extern ShapeVertexEndMode shapeVertexEndMode;
  extern std::vector<BLPointI> shapeVertices;
  extern ColorMode colorModeSetting;
  extern std::array<float, 3> colorMaxSetting;
  extern const std::array<float, 3> colorMaxRgb;
  extern const std::array<float, 3> colorMaxHsb;

  // Create Blend2d Image, ImageBuffer, Context. Set default drawing settings.
  void initContext();
  bool waitSync();
  mfb_update_state updateWindow();
  void resize(struct mfb_window *window, int w, int h);
  BLRgba32 getStrokeColor();
  BLRgba32 getColorFromTripletNormalized(float v1, float v2, float v3);
  BLRgba32 getColorFromTriplet(int v1, int v2, int v3);
  BLRgba32 getColorFromTriplet(float v1, float v2, float v3);
};

int width();
int height();

void size(int w, int h);

// Shape
//  Attributes
// https://processing.org/reference/ellipseMode_.html
void ellipseMode(ShapeAttributesMode mode);
// https://processing.org/reference/rectMode_.html
void rectMode(ShapeAttributesMode mode);
// https://processing.org/reference/strokeCap_.html
void strokeCap(ShapeAttributesCapOrJoin cap);
// https://processing.org/reference/strokeJoin_.html
void strokeJoin(ShapeAttributesCapOrJoin join);
// https://processing.org/reference/strokeWeight_.html
void strokeWeight(double w);
//  Vertex
// https://processing.org/reference/beginShape_.html
void beginShape(ShapeVertexBeginMode mode = POLYGON);
// https://processing.org/reference/vertex_.html
void vertex(int x, int y);
// https://processing.org/reference/endShape_.html
void endShape(ShapeVertexEndMode mode = NONE);
//  2D Primitives
// https://processing.org/reference/circle_.html
void circle(int x, int y, int r);
// https://processing.org/reference/ellipse_.html
void ellipse(int a, int b, int c, int d);
// https://processing.org/reference/line_.html
void line(int x1, int y1, int x2, int y2);
// https://processing.org/reference/point_.html
void point(int x, int y);
// https://processing.org/reference/rect_.html
void rect(int a, int b, int c, int d, int r = 0);
// https://processing.org/reference/square_.html
void square(int x, int y, int s);

// Color
//   Setting
// https://processing.org/reference/background_.html
void background(color c);
void background(uint8_t v1, uint8_t v2, uint8_t v3);
// https://processing.org/reference/colorMode_.html
void colorMode(ColorMode mode, float max = 255.f);
void colorMode(ColorMode mode, float max1, float max2, float max3);
// https://processing.org/reference/fill_.html
void fill(color c);
void fill(uint8_t gray);
void fill(int v1, int v2, int v3);
void fill(float v1, float v2, float v3);
// https://processing.org/reference/noFill_.html
void noFill();
// https://processing.org/reference/noStroke_.html
void noStroke();
// https://processing.org/reference/stroke_.html
void stroke(color c);
void stroke(uint8_t gray);
void stroke(int v1, int v2, int v3);
void stroke(float v1, float v2, float v3);