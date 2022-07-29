#pragma once

#include "Common.h"

namespace ws
{
  class Texture
  {
  public:
    enum class Format
    {
      R32i,
      R32f,
      RGB8,
      RGBA8,
      RGB16f,
      RGBA16f,
      RGB32f,
      RGBA32f,
      Depth32,
      Depth24Stencil8,
    };

    enum class Filter
    {
      Nearest,
      Linear,
    };

    enum class Wrap
    {
      ClampToBorder,
      Repeat,
    };

    struct Specs
    {
      uint32_t width = 1;
      uint32_t height = 1;
      Format format = Format::RGB8;
      Filter filter = Filter::Linear;
      Wrap wrap = Wrap::ClampToBorder;
    };

    Texture();
    Texture(Specs specs);
    ~Texture();

    uint32_t getId() const { return id; }

    Specs specs;

  private:
    uint32_t id = ws::INVALID;
  };
}