#include "Texture.h"

#include <glad/gl.h>
#include <stb_image.h>

#include <cassert>

namespace ws
{
  Texture::GlSpecs Texture::getGlSpecs() const
  {
    GlSpecs gs{};

    switch (specs.format)
    {
    case Format::RGB8:
      gs.internalFormat = GL_RGB;
      gs.format = GL_RGB;
      gs.type = GL_UNSIGNED_BYTE;
      break;
    case Format::Depth24Stencil8:
      gs.internalFormat = GL_DEPTH24_STENCIL8;
      gs.format = GL_DEPTH_STENCIL;
      gs.type = GL_UNSIGNED_INT_24_8;
      break;
    case Format::R32f:
      gs.internalFormat = GL_R32F;
      gs.format = GL_RED;
      gs.type = GL_FLOAT;
      break;
    case Format::RGB32f:
      gs.internalFormat = GL_RGB32F;
      gs.format = GL_RGB;
      gs.type = GL_FLOAT;
      break;
    case Format::RGBA32f:
      gs.internalFormat = GL_RGBA32F;
      gs.format = GL_RGBA;
      gs.type = GL_FLOAT;
      break;
    default:
      assert(false); // missing format conversion
      break;
    };

    switch (specs.filter)
    {
    case Filter::Nearest:
      gs.paramFilter = GL_NEAREST;
      break;
    case Filter::Linear:
      gs.paramFilter = GL_LINEAR;
      break;
    default:
      assert(false); // missing filter conversion
    };

    switch (specs.wrap)
    {
    case Wrap::ClampToBorder:
      gs.paramWrap = GL_CLAMP_TO_BORDER;
      break;
    case Wrap::Repeat:
      gs.paramWrap = GL_REPEAT;
      break;
    default:
      assert(false); // missing wrap conversion
    };

    return gs;
  }

  Texture::Texture() : Texture{Specs{}} {}

  Texture::Texture(const Specs &specs)
      : specs(specs),
        id([]()
           { uint32_t texId; glGenTextures(1, &texId); return texId; }())
  {
    glBindTexture(GL_TEXTURE_2D, id);

    GlSpecs gs = getGlSpecs();
    glTexImage2D(GL_TEXTURE_2D, 0, gs.internalFormat, specs.width, specs.height, 0, gs.format, gs.type, specs.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gs.paramFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gs.paramFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gs.paramWrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gs.paramWrap);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  Texture::Texture(const std::filesystem::path &file)
      : Texture{
            [&file]()
            {
              Specs specs;
              int width, height, nrChannels;
              unsigned char *data = stbi_load(file.string().c_str(), &width, &height, &nrChannels, 0);
              specs.width = width;
              specs.height = height;
              assert(nrChannels == 3);
              specs.wrap = Wrap::Repeat;
              specs.data = data;
              return specs;
            }()}
  {
  }

  void Texture::activateTexture(uint32_t no)
  {
    assert(no == 0); // add more numbers as needed
    glActiveTexture(GL_TEXTURE0);
  }

  void Texture::bind() const
  {
    glBindTexture(GL_TEXTURE_2D, id);
  }

  void Texture::unbind() const
  {
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void Texture::bindImageTexture(uint32_t textureNo, Access access) const
  {
    GLenum glAccess{};
    switch (access)
    {
    case Access::Read:
      glAccess = GL_READ_ONLY;
      break;
    case Access::Write:
      glAccess = GL_WRITE_ONLY;
      break;
    case Access::ReadAndWrite:
      glAccess = GL_READ_WRITE;
      break;
    default:
      assert(false); // unknown Access value
    }
    glBindImageTexture(textureNo, id, 0, GL_FALSE, 0, glAccess, getGlSpecs().internalFormat);
  }

  void Texture::loadPixels(const void *data)
  {
    bind();
    GlSpecs gs = getGlSpecs();
    glTexImage2D(GL_TEXTURE_2D, 0, gs.internalFormat, specs.width, specs.height, 0, gs.format, gs.type, data);
    unbind();
  }

  Texture::~Texture()
  {
    glDeleteTextures(1, &id);
  }
}