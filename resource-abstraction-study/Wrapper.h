#pragma once

#include "ResourceHandler.h"

#include <string>

class Wrapper
{
public:
  int handle = -1; // -1 means uninitialized, -2 means deleted, -3 means moved
  std::string name;

  Wrapper();
  Wrapper(std::string name);
  ~Wrapper();

  Wrapper(const Wrapper &other);
  Wrapper &operator=(const Wrapper &other);

  Wrapper(Wrapper &&other);
  Wrapper &operator=(Wrapper &&other);

private:
  static int counter;
};
