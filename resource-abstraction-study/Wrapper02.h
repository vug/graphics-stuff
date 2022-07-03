#pragma once

#include <string>

class Wrapper
{
public:
  int handle = -1;
  std::string state;

  Wrapper(const std::string &state);
  ~Wrapper();

  // Copy constructor and assignment
  Wrapper(const Wrapper &other);
  Wrapper &operator=(const Wrapper &other);

  // Move constructor and assignment
  Wrapper(Wrapper &&other);
  Wrapper &operator=(Wrapper &&other);

  void use() const;
};