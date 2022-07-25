#pragma once

#include "Common.h"

namespace ws
{
  class Framebuffer
  {
  public:
    // Creates a Framebuffer of size 1x1
    Framebuffer();
    Framebuffer(uint32_t width, uint32_t height);
    ~Framebuffer();

    void bind() const;
    void unbind() const;
    // TODO: add recreateIfNeeded(uint32_t width, uint32_t height) method
    uint32_t getColorAttachment() const;

  private:
    uint32_t fbo{INVALID};
    uint32_t texColor{INVALID};
    uint32_t texDepthStencil{INVALID};
  };
}