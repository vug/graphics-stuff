#include <iostream>

int main()
{
  constexpr auto size{256};

  std::cout << "P3 " << size << " " << size << " 255\n";
  for (auto y = 0; y < size; ++y)
  {
    for (auto x = 0; x < size; ++x)
    {
      const auto r = x;
      const auto g = y;
      const auto b = 0;
      std::cout << r << " " << g << " " << b << " ";
    }
    std::cout << "\n"; // not needed
  }
}