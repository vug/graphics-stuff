#pragma once

#include <string>

class Wrapper
{
public:
  int handle = -1; // -1 means uninitialized, -2 means deleted, -3 means moved
  std::string state;

  Wrapper(const std::string &state);
  ~Wrapper();

  void use() const;
};