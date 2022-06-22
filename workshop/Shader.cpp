#include <glad/gl.h>

#include "Shader.h"

#include <iostream>

namespace ws
{
  Shader::Shader(const char *vertexShaderSource, const char *fragmentShaderSource)
  {
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexShaderSource, NULL);
    glCompileShader(vertex);
    int success;
    char infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(vertex, 512, NULL, infoLog);
      std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                << infoLog << std::endl;
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
    }
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertex);
    glAttachShader(shaderProgram, fragment);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                << infoLog << std::endl;
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    id = shaderProgram;
  }

  Shader::~Shader()
  {
    glDeleteProgram(id);
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