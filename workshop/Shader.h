#pragma once

// TODO: for uint32_t. try removing later
#include <cstdint>

namespace ws
{
  class Shader
  {
  public:
    Shader(const char *vertexShaderSource, const char *fragmentShaderSource);
    ~Shader();

    void setVector3fv(int32_t shaderId, const char *name, float *value);
    void setMatrix3fv(int32_t shaderId, const char *name, float *value);
    void setMatrix4fv(int32_t shaderId, const char *name, float *value);
    void blockBinding(int32_t shaderId, const char *name, uint32_t binding);

  public:
    int32_t id{-1};
  };
}