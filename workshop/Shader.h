#pragma once

// TODO: for uint32_t. try removing later
#include <cstdint>
#include <filesystem>

namespace ws
{
  class Shader
  {
  public:
    // Just acquires a Shader Program Id from OpenGL context
    Shader();
    // Create a shader program and compile shaders source codes
    Shader(const char *vertexShaderSource, const char *fragmentShaderSource);
    // Create a shader program and compile shaders from files. Keep track of files for further reload
    Shader(std::filesystem::path vertexShader, std::filesystem::path fragmentShader);
    // Deallocate resources
    ~Shader();

    void setVector2fv(const char *name, const float *value);
    void setVector3fv(const char *name, const float *value);
    void setMatrix3fv(const char *name, const float *value);
    void setMatrix4fv(const char *name, const float *value);
    void blockBinding(const char *name, uint32_t binding);

    // Compiles shader sources into same program. Good for hot-reload
    bool compile(const char *vertexShaderSource, const char *fragmentShaderSource);
    // Reuse program with new shader files
    bool load(std::filesystem::path vertexShader, std::filesystem::path fragmentShader);
    // reload/recompile same shader files. keep shader program id. Good for hot-reload.
    // needs shader files to be reloaded first. otherwise fails.
    bool reload();

    inline int32_t getId() const
    {
      return id;
    }
    // Whether a functioning shader program was created or not
    // i.e. shaders compiled and linked successfully, not "id != -1"
    bool isValid();
    // asserts validity then binds
    void bind();

  public:
  private:
    std::filesystem::path vertexShader;
    std::filesystem::path fragmentShader;
    int32_t id{-1};
    bool valid{};
  };
}