#pragma once
#include <MiniFB.h>

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

  bool waitSync();
  mfb_update_state updateWindow();
  void resize(struct mfb_window *window, int w, int h);
};

int width();
int height();
void size(int w, int h);
void point(int x, int y, Color c);