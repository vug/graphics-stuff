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
