#include "Framebuffer.h"

#include <glad/gl.h>

#include <cstdio>

namespace ws
{
  Framebuffer::Framebuffer(uint32_t width, uint32_t height)
  {
    glGenFramebuffers(1, &fbo);
    bind();

    // Texture
    glGenTextures(1, &texColor);
    glBindTexture(GL_TEXTURE_2D, texColor);
    // TODO: parametrize format
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    // TODO: paremetrize filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &texDepthStencil);
    glBindTexture(GL_TEXTURE_2D, texDepthStencil);
    // TODO: parametrize attachment no and texture type
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColor, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texDepthStencil, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      printf("Framebuffer %u incomplete.", fbo);

    unbind();
  }

  Framebuffer::Framebuffer() : Framebuffer(1, 1) {}

  Framebuffer::~Framebuffer()
  {
    glDeleteFramebuffers(1, &fbo);
  }

  void Framebuffer::bind() const
  {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  }

  void Framebuffer::unbind() const
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  uint32_t Framebuffer::getColorAttachment() const
  {
    return texColor;
  }
}