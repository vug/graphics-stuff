#pragma once

#include <cstdint>

namespace ws
{
  class Framebuffer
  {
  public:
    Framebuffer();
    ~Framebuffer();

    void bind() const;
    void unbind() const;

  private:
    uint32_t fbo{};
  };
}