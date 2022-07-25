
#include "Shader.h"

#include <glad/gl.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

namespace ws
{
  std::string readFile(std::filesystem::path fp)
  {
    std::ifstream stream(fp, std::ios::in | std::ios::binary);
    const auto size = std::filesystem::file_size(fp);
    std::string content(size, '\0');
    stream.read(content.data(), size);
    return content;
  }

  Shader::Shader()
      : id(glCreateProgram()) {}

  Shader::Shader(const char *vertexShaderSource, const char *fragmentShaderSource)
      : id(glCreateProgram())
  {
    compile(vertexShaderSource, fragmentShaderSource);
  }

  Shader::Shader(std::filesystem::path vertexShader, std::filesystem::path fragmentShader)
      : vertexShader(vertexShader), fragmentShader(fragmentShader), id(glCreateProgram())
  {
    load(vertexShader, fragmentShader);
  }

  bool Shader::compile(const char *vertexShaderSource, const char *fragmentShaderSource)
  {
    int success;
    char infoLog[512];

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShaderSource, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(vertex, 512, NULL, infoLog);
      std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                << infoLog << std::endl;
      return success;
    }

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(fragment, 512, NULL, infoLog);
      std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                << infoLog << std::endl;

      glDeleteShader(vertex);
      return success;
    }

    if (isValid())
      detachShaders();

    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetProgramInfoLog(id, 512, NULL, infoLog);
      std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                << infoLog << std::endl;

      glDeleteShader(vertex);
      glDeleteShader(fragment);
      return success;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return success;
  }

  bool Shader::load(std::filesystem::path vertexShader, std::filesystem::path fragmentShader)
  {
    this->vertexShader = vertexShader;
    this->fragmentShader = fragmentShader;

    if (vertexShader.empty() || fragmentShader.empty())
    {
      std::cerr << "shader object " << id << " is not associated with a shader file\n";
      return false;
    }
    else if (!std::filesystem::exists(vertexShader) || !std::filesystem::exists(fragmentShader))
    {
      std::cerr << "no shader file: " << vertexShader.string() << " or: " << fragmentShader.string() << "\n";
      return false;
    }

    const std::string vertexCode = readFile(vertexShader);
    const std::string fragmentCode = readFile(fragmentShader);

    return compile(vertexCode.c_str(), fragmentCode.c_str());
  }

  bool Shader::reload()
  {
    return load(vertexShader, fragmentShader);
  }

  Shader::~Shader()
  {
    if (isValid())
      detachShaders();
    glDeleteProgram(id);
  }

  bool Shader::isValid() const
  {
    int valid{};
    glGetProgramiv(id, GL_LINK_STATUS, &valid);
    return valid;
  }

  void Shader::bind() const
  {
    assert(isValid());
    glUseProgram(id);
  }

  void Shader::unbind() const
  {
    glUseProgram(0);
  }

  std::vector<uint32_t> Shader::getShaderIds() const
  {
    const int maxShaders = 2;
    std::vector<uint32_t> shaderIds(maxShaders);

    int count{};
    glGetAttachedShaders(id, 2, &count, shaderIds.data());
    shaderIds.resize(count);

    return shaderIds;
  }

  void Shader::detachShaders()
  {
    auto shaderIds = getShaderIds();
    for (auto shaderId : shaderIds)
      glDetachShader(id, shaderId);
  }

  void Shader::SetScalar1f(const char *name, const float value)
  {
    const int location = glGetUniformLocation(id, name);
    glUniform1f(location, value);
  }

  void Shader::setVector2fv(const char *name, const float *value)
  {
    const int location = glGetUniformLocation(id, name);
    glUniform2fv(location, 1, value);
  }
  void Shader::setVector3fv(const char *name, const float *value)
  {
    const int location = glGetUniformLocation(id, name);
    glUniform3fv(location, 1, value);
  }
  void Shader::setMatrix3fv(const char *name, const float *value)
  {
    const int location = glGetUniformLocation(id, name);
    glUniformMatrix3fv(location, 1, GL_FALSE, value);
  }
  void Shader::setMatrix4fv(const char *name, const float *value)
  {
    const int location = glGetUniformLocation(id, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, value);
  }
  void Shader::blockBinding(const char *name, uint32_t binding)
  {
    unsigned int index = glGetUniformBlockIndex(id, name);
    glUniformBlockBinding(id, index, binding);
  }
}