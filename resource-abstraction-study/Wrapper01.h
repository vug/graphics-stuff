#pragma once

#include <string>

class Wrapper
{
public:
  int handle = -1; // -1 means, uninitialized
  std::string state;

  Wrapper(const std::string &state);
  ~Wrapper();

  void use() const;
};