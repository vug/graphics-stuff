#pragma once

#include "color.h"

#include <cmath>
#include <iostream>
#include <vector>

class Context
{
private:
  int width_ = 512;
  int height_ = 512;

public:
  std::vector<std::vector<Color>> pixels;

public:
  Context(int width, int height) : width_(width), height_(height)
  {
    for (int j = 0; j < height; j++)
      pixels.emplace_back(width);
  }

  void lineBresenham(int x0, int y0, int x1, int y1, Color c)
  {
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */

    for (;;)
    {
      pixels[y0][x0] = c;
      if (x0 == x1 && y0 == y1)
        break;
      e2 = 2 * err;
      if (e2 >= dy)
      {
        err += dy;
        x0 += sx;
      } /* e_xy+e_x > 0 */
      if (e2 <= dx)
      {
        err += dx;
        y0 += sy;
      } /* e_xy+e_y < 0 */
    }
  }

  void printPPM()
  {
    std::cout << "P3 " << width_ << " " << height_ << " 255\n";
    for (auto &row : pixels)
    {
      for (auto &color : row)
        std::cout << color.r << " " << color.g << " " << color.b << " ";
      std::cout << "\n";
    }
  }
};
