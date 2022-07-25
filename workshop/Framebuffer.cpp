#include "Framebuffer.h"

#include <glad/gl.h>

#include <cstdio>

namespace ws
{
  Framebuffer::Framebuffer()
  {
    glGenFramebuffers(1, &fbo);
    bind();

    // Texture
    uint32_t texColor;
    glGenTextures(1, &texColor);
    glBindTexture(GL_TEXTURE_2D, texColor);
    // TODO: update width, height, parametrize format
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    // TODO: paremetrize filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glBindTexture(GL_TEXTURE_2D, 0);

    uint32_t texDepthStencil;
    glGenTextures(1, &texDepthStencil);
    glBindTexture(GL_TEXTURE_2D, texDepthStencil);
    // TODO: parametrize attachment no and texture type
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColor, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texDepthStencil, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
      printf("Framebuffer %u completed", fbo);
    }
    else
      printf("Framebuffer %u incomplete.", fbo);

    unbind();
  }

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
}