#pragma once
#include <MiniFB.h>
#define BL_STATIC
#include <blend2d.h>

struct Color
{
  uint8_t r = 255;
  uint8_t g = 255;
  uint8_t b = 255;
};

enum ShapeAttributesMode {
  CORNER = 0,
  CORNERS = 1,
  CENTER = 2,
  RADIUS = 3,
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

  // Create Blend2d Image, ImageBuffer, Context. Set default drawing settings.
  void initContext();
  bool waitSync();
  mfb_update_state updateWindow();
  void resize(struct mfb_window *window, int w, int h);
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
//  2D Primitives
// https://processing.org/reference/point_.html
void point(int x, int y, Color c);
// https://processing.org/reference/rect_.html
void rect(int a, int b, int c, int d, int r = 0);

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