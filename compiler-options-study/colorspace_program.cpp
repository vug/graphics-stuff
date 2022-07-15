#include "ColorSpace.h"

#include <cstdio>

int main()
{
  ColorSpace::Rgb rgb{0.7, 0.6, 0.5};
  ColorSpace::Hsv hsv{};
  rgb.To(&hsv);
  printf("%f %f %f", hsv.h, hsv.s, hsv.v);
}