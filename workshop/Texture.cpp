#include "Texture.h"

#include <glad/gl.h>

namespace ws
{
  Texture::Texture() : Texture{Specs{}} {}
  Texture::Texture(Specs specs)
      : specs(specs),
        id([]()
           { uint32_t texId; glGenTextures(1, &texId); return texId; }())
  {
    glBindTexture(GL_TEXTURE_2D, id);
    GLint internalFormat = -1;
    GLenum format = -1;
    GLenum type = -1;
    switch (specs.format)
    {
    case Format::RGB8:
      internalFormat = GL_RGB;
      format = GL_RGB;
      type = GL_UNSIGNED_BYTE;
      break;
    case Format::Depth24Stencil8:
      internalFormat = GL_DEPTH24_STENCIL8;
      format = GL_DEPTH_STENCIL;
      type = GL_UNSIGNED_INT_24_8;
    };

    GLint paramFilter = -1;
    switch (specs.filter)
    {
    case Filter::Nearest:
      paramFilter = GL_NEAREST;
      break;
    case Filter::Linear:
      paramFilter = GL_LINEAR;
      break;
    };

    GLint paramWrap = -1;
    switch (specs.wrap)
    {
    case Wrap::ClampToBorder:
      paramWrap = GL_CLAMP_TO_BORDER;
      break;
    case Wrap::Repeat:
      paramWrap = GL_REPEAT;
      break;
    };
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, specs.width, specs.height, 0, format, type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, paramFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, paramFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, paramWrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, paramWrap);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  Texture::~Texture()
  {
    glDeleteTextures(1, &id);
  }
}