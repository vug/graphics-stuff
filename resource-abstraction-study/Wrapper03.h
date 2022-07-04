#pragma once

#include "ResourceHandler.h"

#include <string>

class Wrapper
{
public:
  int handle = -1; // -1 means uninitialized, -2 means deleted, -3 means moved
  std::string state;

  Wrapper();
  Wrapper(std::string state);
  ~Wrapper();

  Wrapper(const Wrapper &other) = delete;
  Wrapper &operator=(const Wrapper &other) = delete;

  Wrapper(Wrapper &&other);
  Wrapper &operator=(Wrapper &&other);

  void use() const;
};
