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
      return lineHorizontal(x0, x1, y0, c);
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

  /**
   * This is a variation on naive algorithm where we don't calculate the line formula
   * for every pixel, instead increment the non-looping coordinate when needed
   */
  void lineBresenham(int x0, int y0, int x1, int y1, Color c)
  {
    const int deltaX = x1 - x0;
    const int deltaY = y1 - y0;
    if (deltaY == 0)
      return lineHorizontal(x0, x1, y0, c);
    if (deltaX == 0)
    {
      lineVertical(y0, y1, x0, c);
      return;
    }

    const bool isSteep = std::abs(deltaY) > std::abs(deltaX);
    int dy = y1 >= y0 ? +1 : -1;
    int dx = x1 >= x0 ? +1 : -1;
    float accumulate = 0.0f;
    float threshold = 0.5f;

    if (!isSteep)
    {
      const float m = static_cast<float>(deltaY) / deltaX;
      const float mAbs = std::abs(m);
      int y = y0;
      for (int x = x0; x != x1; x += dx)
      {
        if (accumulate > threshold)
        {
          y += dy;
          threshold += 1;
        }
        pixels[y][x] = c;
        accumulate += mAbs;
      }
    }
    else
    {
      const float mInv = static_cast<float>(deltaX) / deltaY;
      const float mInvAbs = std::abs(mInv);
      int x = x0;
      for (int y = y0; y != y1; y += dy)
      {
        if (accumulate > threshold)
        {
          x += dx;
          threshold += 1;
        }
        pixels[y][x] = c;
        accumulate += mInvAbs;
      }
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

  /**
   * Drawing functions by Alois Zingl
   * http://members.chello.at/~easyfilter/bresenham.html
   * http://members.chello.at/~easyfilter/Bresenham.pdf
   */
  void zinglLine(int x0, int y0, int x1, int y1, Color c)
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

  void zinglLineAA(int x0, int y0, int x1, int y1, Color c)
  {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx - dy, e2, x2; /* error value e_xy */
    int ed = dx + dy == 0 ? 1 : static_cast<int>(sqrt((float)dx * dx + (float)dy * dy));

    for (;;)
    {
      pixels[y0][x0] = Color::interpolate(c, pixels[y0][x0], std::abs(err - dx + dy) / static_cast<float>(ed));
      e2 = err;
      x2 = x0;
      if (2 * e2 >= -dx)
      { /* x step */
        if (x0 == x1)
          break;
        if (e2 + dy < ed)
          pixels[y0 + sy][x0] = Color::interpolate(c, pixels[y0 + sy][x0], std::abs(e2 + dy) / static_cast<float>(ed));
        err -= dy;
        x0 += sx;
      }
      if (2 * e2 <= dy)
      { /* y step */
        if (y0 == y1)
          break;
        if (dx - e2 < ed)
          pixels[y0][x2 + sx] = Color::interpolate(c, pixels[y0][x2 + sx], std::abs(dx - e2) / static_cast<float>(ed));
        err += dx;
        y0 += sy;
      }
    }
  }

  void zinglLineWidthAA(int x0, int y0, int x1, int y1, float wd, Color c)
  {
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx - dy, e2, x2, y2; /* error value e_xy */
    float ed = dx + dy == 0 ? 1 : sqrt((float)dx * dx + (float)dy * dy);

    for (wd = (wd + 1) / 2;;)
    {
      pixels[y0][x0] = Color::interpolate(c, pixels[y0][x0], std::fmax<float>(0, (abs(err - dx + dy) / ed - wd + 1))); /* pixel loop */
      e2 = err;
      x2 = x0;
      if (2 * e2 >= -dx)
      { /* x step */
        for (e2 += dy, y2 = y0; e2 < ed * wd && (y1 != y2 || dx > dy); e2 += dx)
        {
          y2 += sy;
          pixels[y2][x0] = Color::interpolate(c, pixels[y2][x0], std::fmax<float>(0, (abs(e2) / ed - wd + 1)));
        }
        if (x0 == x1)
          break;
        e2 = err;
        err -= dy;
        x0 += sx;
      }
      if (2 * e2 <= dy)
      { /* y step */
        for (e2 = dx - e2; e2 < ed * wd && (x1 != x2 || dx < dy); e2 += dy)
        {
          x2 += sx;
          pixels[y0][x2] = Color::interpolate(c, pixels[y0][x2], std::fmax<float>(0, (abs(e2) / ed - wd + 1)));
        }
        if (y0 == y1)
          break;
        err += dx;
        y0 += sy;
      }
    }
  }

  void zinglCircle(int xm, int ym, int r, Color c)
  {
    int x = -r, y = 0, err = 2 - 2 * r; /* II. Quadrant */
    do
    {
      pixels[ym + y][xm - x] = c; /*   I. Quadrant */
      pixels[ym - x][xm - y] = c; /*  II. Quadrant */
      pixels[ym - y][xm + x] = c; /* III. Quadrant */
      pixels[ym + x][xm + y] = c; /*  IV. Quadrant */
      r = err;
      if (r <= y)
        err += ++y * 2 + 1; /* e_xy+e_y < 0 */
      if (r > x || err > y)
        err += ++x * 2 + 1; /* e_xy+e_x > 0 or no 2nd y-step */
    } while (x < 0);
  }

  void zinglEllipseRect(int x0, int y0, int x1, int y1, Color c)
  {
    int a = std::abs(x1 - x0), b = std::abs(y1 - y0), b1 = b & 1; /* values of diameter */
    long dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a;     /* error increment */
    long err = dx + dy + b1 * a * a, e2;                          /* error of 1.step */

    if (x0 > x1)
    {
      x0 = x1;
      x1 += a;
    } /* if called with swapped points */
    if (y0 > y1)
      y0 = y1; /* .. exchange them */
    y0 += (b + 1) / 2;
    y1 = y0 - b1; /* starting pixel */
    a *= 8 * a;
    b1 = 8 * b * b;

    do
    {
      pixels[y0][x1] = c; /*   I. Quadrant */
      pixels[y0][x0] = c; /*  II. Quadrant */
      pixels[y1][x0] = c; /* III. Quadrant */
      pixels[y1][x1] = c; /*  IV. Quadrant */
      e2 = 2 * err;
      if (e2 <= dy)
      {
        y0++;
        y1--;
        err += dy += a;
      } /* y step */
      if (e2 >= dx || 2 * err > dy)
      {
        x0++;
        x1--;
        err += dx += b1;
      } /* x step */
    } while (x0 <= x1);

    while (y0 - y1 < b)
    {                         /* too early stop of flat ellipses a=1 */
      pixels[x0 - 1][y0] = c; /* -> finish tip of ellipse */
      pixels[x1 + 1][y0++] = c;
      pixels[x0 - 1][y1] = c;
      pixels[x1 + 1][y1--] = c;
    }
  }

  void zinglQuadBezierSeg(int x0, int y0, int x1, int y1, int x2, int y2, Color c)
  {
    int sx = x2 - x1, sy = y2 - y1;
    long xx = x0 - x1, yy = y0 - y1, xy;         /* relative values for checks */
    double dx, dy, err, cur = xx * sy - yy * sx; /* curvature */

    assert(xx * sx <= 0 && yy * sy <= 0); /* sign of gradient must not change */

    if (sx * (long)sx + sy * (long)sy > xx * xx + yy * yy)
    { /* begin with longer part */
      x2 = x0;
      x0 = sx + x1;
      y2 = y0;
      y0 = sy + y1;
      cur = -cur; /* swap P0 P2 */
    }
    if (cur != 0)
    { /* no straight line */
      xx += sx;
      xx *= sx = x0 < x2 ? 1 : -1; /* x step direction */
      yy += sy;
      yy *= sy = y0 < y2 ? 1 : -1; /* y step direction */
      xy = 2 * xx * yy;
      xx *= xx;
      yy *= yy; /* differences 2nd degree */
      if (cur * sx * sy < 0)
      { /* negated curvature? */
        xx = -xx;
        yy = -yy;
        xy = -xy;
        cur = -cur;
      }
      dx = 4.0 * sy * cur * (x1 - x0) + xx - xy; /* differences 1st degree */
      dy = 4.0 * sx * cur * (y0 - y1) + yy - xy;
      xx += xx;
      yy += yy;
      err = dx + dy + xy; /* error 1st step */
      do
      {
        pixels[y0][x0] = c; /* plot curve */
        if (x0 == x2 && y0 == y2)
          return;          /* last pixel -> curve finished */
        y1 = 2 * err < dx; /* save value for test of y step */
        if (2 * err > dy)
        {
          x0 += sx;
          dx -= xy;
          err += dy += yy;
        } /* x step */
        if (y1)
        {
          y0 += sy;
          dy -= xy;
          err += dx += xx;
        }                /* y step */
      } while (dy < dx); /* gradient negates -> algorithm fails */
    }
    zinglLine(x0, y0, x2, y2, c); /* plot remaining part to end */
  }

  void zinglQuadBezierSegAA(int x0, int y0, int x1, int y1, int x2, int y2, Color c)
  {
    int sx = x2 - x1, sy = y2 - y1;
    long xx = x0 - x1, yy = y0 - y1, xy;             /* relative values for checks */
    double dx, dy, err, ed, cur = xx * sy - yy * sx; /* curvature */

    assert(xx * sx <= 0 && yy * sy <= 0); /* sign of gradient must not change */

    if (sx * (long)sx + sy * (long)sy > xx * xx + yy * yy)
    { /* begin with longer part */
      x2 = x0;
      x0 = sx + x1;
      y2 = y0;
      y0 = sy + y1;
      cur = -cur; /* swap P0 P2 */
    }
    if (cur != 0)
    { /* no straight line */
      xx += sx;
      xx *= sx = x0 < x2 ? 1 : -1; /* x step direction */
      yy += sy;
      yy *= sy = y0 < y2 ? 1 : -1; /* y step direction */
      xy = 2 * xx * yy;
      xx *= xx;
      yy *= yy; /* differences 2nd degree */
      if (cur * sx * sy < 0)
      { /* negated curvature? */
        xx = -xx;
        yy = -yy;
        xy = -xy;
        cur = -cur;
      }
      dx = 4.0 * sy * (x1 - x0) * cur + xx - xy; /* differences 1st degree */
      dy = 4.0 * sx * (y0 - y1) * cur + yy - xy;
      xx += xx;
      yy += yy;
      err = dx + dy + xy; /* error 1st step */
      do
      {
        cur = fmin(dx + xy, -xy - dy);
        ed = fmax(dx + xy, -xy - dy); /* approximate error distance */
        ed = 255 / (ed + 2 * ed * cur * cur / (4. * ed * ed + cur * cur));
        pixels[y0][x0] = Color::interpolate(c, pixels[y0][x0], static_cast<float>(ed * std::fabs(err - dx - dy - xy) / 255.));
        if (x0 == x2 && y0 == y2)
          return; /* last pixel -> curve finished */
        x1 = x0;
        cur = dx - err;
        y1 = 2 * err + dy < 0;
        if (2 * err + dx > 0)
        { /* x step */
          if (err - dy < ed)
            pixels[y0 + sy][x0] = Color::interpolate(c, pixels[y0 + sy][x0], static_cast<float>(ed * std::fabs(err - dy) / 255.));
          x0 += sx;
          dx -= xy;
          err += dy += yy;
        }
        if (y1)
        { /* y step */
          if (cur < ed)
            pixels[y0][x1 + sx] = Color::interpolate(c, pixels[y0 + sy][x1 + sx], static_cast<float>(ed * std::fabs(cur) / 255.));
          y0 += sy;
          dy -= xy;
          err += dx += xx;
        }
      } while (dy < dx); /* gradient negates -> close curves */
    }
    zinglLineAA(x0, y0, x2, y2, c); /* plot remaining needle to end */
  }
};
