#pragma once

#include <memory>
#include <string>

class Wrapper
{
private:
  int handle = -1;
  Wrapper(const std::string &state);

public:
  std::string state;
  ~Wrapper();

  static std::shared_ptr<Wrapper> makeWrapper(const std::string &state);

  Wrapper(const Wrapper &other) = delete;
  Wrapper &operator=(const Wrapper &other) = delete;
  Wrapper(Wrapper &&other) = delete;
  Wrapper &operator=(Wrapper &&other) = delete;

  void use() const;
};