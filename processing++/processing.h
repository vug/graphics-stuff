#pragma once
#include <MiniFB.h>
#define BL_STATIC
#include <blend2d.h>

#include <vector>

struct Color
{
  uint8_t r = 255;
  uint8_t g = 255;
  uint8_t b = 255;
};

enum ShapeAttributesCap
{
  ROUND = 0,
  SQUARE = 1,
  PROJECT = 2,
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
  QUAD_STRIP
};

enum ShapeVertexEndMode
{
  NONE = 0,
  CLOSE = 1,
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
  extern ShapeAttributesCap shapeCap;
  extern ShapeVertexBeginMode shapeVertexBeginMode;
  extern ShapeVertexEndMode shapeVertexEndMode;
  extern std::vector<BLPointI> shapeVertices;

  // Create Blend2d Image, ImageBuffer, Context. Set default drawing settings.
  void initContext();
  bool waitSync();
  mfb_update_state updateWindow();
  void resize(struct mfb_window *window, int w, int h);
  BLRgba32 getStrokeColor();
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
void strokeCap(ShapeAttributesCap cap);
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
void background(uint8_t r, uint8_t g, uint8_t b);
// https://processing.org/reference/fill_.html
void fill(uint8_t gray);
void fill(uint8_t r, uint8_t g, uint8_t b);
// https://processing.org/reference/noFill_.html
void noFill();
// https://processing.org/reference/noStroke_.html
void noStroke();
// https://processing.org/reference/stroke_.html
void stroke(uint8_t r, uint8_t g, uint8_t b);