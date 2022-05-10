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

  void lineHorizontal(int x0, int x1, int y, Color c)
  {
    assert(x0 >= 0 && x0 < width_ && x1 >= 0 && x1 < width_ && y >= 0 && y < height_);
    if (x1 < x0)
      std::swap(x0, x1);

    for (int x = x0; x <= x1; ++x)
      pixels[y][x] = c;
  }

  void lineVertical(int y0, int y1, int x, Color c)
  {
    assert(y0 >= 0 && y0 < height_ && y1 >= 0 && y1 < height_ && x >= 0 && x < width_);
    if (y1 < y0)
      std::swap(y0, y1);

    for (int y = y0; y <= y1; ++y)
      pixels[y][x] = c;
  }

  /**
   * Naive line drawing is about coloring each pixel on the longer side
   * and using the linear line equation y = m * x + b
   * steep means y-axis has the longer side.
   * while looping over longer-side axis use the line equation to get the coordinate for other axis
   */
  void lineNaive(int x0, int y0, int x1, int y1, Color c)
  {
    const int deltaX = x1 - x0;
    const int deltaY = y1 - y0;
    if (deltaY == 0)
    {
      lineHorizontal(x0, x1, y0, c);
      return;
    }
    if (deltaX == 0)
    {
      lineVertical(y0, y1, x0, c);
      return;
    }

    const float m = static_cast<float>(deltaY) / deltaX;
    const float b = y0 - m * x0;
    const bool isSteep = std::abs(deltaY) > std::abs(deltaX);
    if (!isSteep)
    {
      if (x1 < x0)
        std::swap(x0, x1);
      for (int x = x0; x <= x1; ++x)
      {
        const float yf = m * x + b; // y from x
        const int y = static_cast<int>(std::round(yf));
        pixels[y][x] = c;
      }
    }
    else
    {
      if (y1 < y0)
        std::swap(y0, y1);
      for (int y = y0; y <= y1; ++y)
      {
        const float xf = (y - b) / m; // x from y
        const int x = static_cast<int>(std::round(xf));
        pixels[y][x] = c;
      }
    }
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
