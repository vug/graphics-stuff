/**
 * Icosahedron mesh generation. GLM camera orbit around it.
 */
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void makeIcosahedron(std::vector<float> &verts, std::vector<unsigned int> &idxs);

const char *vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";
const char *fragmentShaderSource = R"(
#version 460 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)";

int width = 800;
int height = 600;

int numMaxVertices = 64;
std::vector<float> vertices = {
    0.0f, 0.0f, 0.0f, // p1
    0.0f, 0.1f, 0.0f, // p2
    0.1f, 0.0f, 0.0f, // p3
};
std::vector<unsigned int> indices = {
    0, 1, 2, // triangle1
};
unsigned int vbo, vao, ebo;

int main()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 8);

  GLFWwindow *window = glfwCreateWindow(width, height, "camera orbits around an icosahedron", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetKeyCallback(window, key_callback);

  gladLoadGL();

  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }
  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  makeIcosahedron(vertices, indices);

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glBindVertexArray(vao);
  // Allocate 16 vertices worth memory initially
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numMaxVertices * 3, nullptr, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numMaxVertices, nullptr, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertices.size(), vertices.data());
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * indices.size(), indices.data());
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  // glEnable(GL_LINE_SMOOTH);
  // glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  // glLineWidth(1.0f);
  glEnable(GL_MULTISAMPLE);

  while (!glfwWindowShouldClose(window))
  {
    double t = glfwGetTime();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glm::vec3 cameraPos = glm::vec3(std::cos(t), 1.0f, std::sin(t));
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    const float fov = 45.0f;
    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, up);
    glm::mat4 projection = glm::perspective(glm::radians(fov), static_cast<float>(width) / height, 0.1f, 100.0f);
    const int viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    const int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    glm::mat4 model = glm::mat4(1.0f);
    const int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}

void key_callback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mode)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  if (key == GLFW_KEY_T && action == GLFW_PRESS)
  {
    const size_t lastIx = vertices.size();
    for (size_t ix = 0; ix < 9; ++ix)
      vertices.push_back(vertices[lastIx - 9 + ix] + (ix % 3 == 2 ? 0.0f : 0.05f));
    for (int i = 0; i < 3; ++i)
      indices.push_back(static_cast<unsigned int>(indices.size()));

    // If allocated memory is enough, just use glBufferSubData for fast upload
    // When not enough, use glBufferData double memory size
    if (indices.size() <= numMaxVertices)
    {
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertices.size(), vertices.data());
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
      glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * indices.size(), indices.data());
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else
    {
      // std::cout << "num vertices: " << indices.size() << ", max vertices: " << numMaxVertices << ". Amortize!\n";
      numMaxVertices *= 2;
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numMaxVertices * 3, vertices.data(), GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numMaxVertices, indices.data(), GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
  }
}

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, int w, int h)
{
  width = w;
  height = h;
  glViewport(0, 0, width, height);
}

void makeIcosahedron(std::vector<float> &verts, std::vector<unsigned int> &idxs)
{
  verts.clear();
  idxs.clear();

  float phi = (1.0f + sqrt(5.0f)) * 0.5f; // golden ratio
  float a = 1.0f;
  float b = 1.0f / phi;

  std::vector<glm::vec3> vec3s = {
      glm::vec3(0, b, -a),
      glm::vec3(b, a, 0),
      glm::vec3(-b, a, 0),
      glm::vec3(0, b, a),
      glm::vec3(0, -b, a),
      glm::vec3(-a, 0, b),
      glm::vec3(0, -b, -a),
      glm::vec3(a, 0, -b),
      glm::vec3(a, 0, b),
      glm::vec3(-a, 0, -b),
      glm::vec3(b, -a, 0),
      glm::vec3(-b, -a, 0),
  };

  for (auto &v : vec3s)
  {
    v /= v.length();
    vertices.push_back(v.x);
    vertices.push_back(v.y);
    vertices.push_back(v.z);
  }

  indices = {
      2, 1, 0,   // 0
      1, 2, 3,   // 1
      5, 4, 3,   //
      4, 8, 3,   //
      7, 6, 0,   //
      6, 9, 0,   //
      11, 10, 4, //
      10, 11, 6, //
      9, 5, 2,   //
      5, 9, 11,  //
      8, 7, 1,   //
      7, 8, 10,  //
      2, 5, 3,   //
      8, 1, 3,   //
      9, 2, 0,   //
      1, 7, 0,   //
      11, 9, 6,  //
      7, 10, 6,  //
      5, 11, 4,  //
      10, 8, 4,  //
  };
}