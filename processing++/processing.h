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

  void initImageBuffer();
  bool waitSync();
  mfb_update_state updateWindow();
  void resize(struct mfb_window *window, int w, int h);
};

int width();
int height();

void size(int w, int h);
void point(int x, int y, Color c);

// Color
//   Setting
// https://processing.org/reference/background_.html
void background(int8_t r, int8_t g, int8_t b);