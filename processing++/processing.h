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

  void initImageBuffer();
  bool waitSync();
  mfb_update_state updateWindow();
  void resize(struct mfb_window *window, int w, int h);
};

int width();
int height();

void size(int w, int h);
void point(int x, int y, Color c);

// Shape
//  2D Primitives
// https://processing.org/reference/rect_.html
void rect(int x, int y, int w, int h);

// Color
//   Setting
// https://processing.org/reference/background_.html
void background(uint8_t r, uint8_t g, uint8_t b);
// https://processing.org/reference/fill_.html
void fill(uint8_t r, uint8_t g, uint8_t b);
// https://processing.org/reference/noFill_.html
void noFill();
// https://processing.org/reference/noStroke_.html
void noStroke();
// https://processing.org/reference/stroke_.html
void stroke(uint8_t r, uint8_t g, uint8_t b);