#include "color.h"

#include <complex>
#include <iostream>

int main(int argc, char* argv[])
{
  constexpr auto size{1024};
  constexpr Color red{255, 0, 0};
  constexpr Color blue{0, 0, 255};
  constexpr auto max_iteration{32};

  // parse arguments
  double centerX = 0.0;
  double centerY = 0.0;
  double side = 1.0;
  if (argc == 4) {
    centerX = atof(argv[1]);
    centerY = atof(argv[2]);
    side = atof(argv[3]);
  }

  std::cout << "P3 " << size << " " << size << " 255\n";
  for (auto j = 0; j < size; ++j)
  {
    for (auto i = 0; i < size; ++i)
    {
      // gradient
      // const Color c = Color::interpolate(red, blue, static_cast<float>(y) / size);

      double x = centerX - side * 0.5 + side / size * i;
      double y = centerY - side * 0.5 + side / size * j;

      const std::complex<double> c0{x, y};
      std::complex<double> z{0.0, 0.0};
      auto k = 0;
      for (; k < max_iteration; ++k)
      {
        z = z * z + c0;
        if (std::abs(z) >= 2.0)
          break;
      }
      const Color c = Color::interpolate(blue, red, static_cast<float>(k) / max_iteration);

      std::cout << c.r << " " << c.g << " " << c.b << " ";
    }
    std::cout << "\n"; // not necessary for PPM
  }
}