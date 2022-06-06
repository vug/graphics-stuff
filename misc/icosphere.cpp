/**
 * Icosphere by subdividing icosahedron
 */
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include <iostream>
#include <vector>

using MyMesh = OpenMesh::TriMesh_ArrayKernelT<>;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void makeIcosahedronOMesh(MyMesh &oMesh);

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

  MyMesh oMesh;
  makeIcosahedronOMesh(oMesh);

  std::vector<float> vertices = {};
  std::vector<unsigned int> indices = {};
  unsigned int vbo, vao, ebo;

  for (const auto &v : oMesh.vertices())
  {
    const auto &p = oMesh.point(v);
    vertices.push_back(p[0]);
    vertices.push_back(p[1]);
    vertices.push_back(p[2]);
  }

  for (auto f : oMesh.faces())
    for (auto v : f.vertices())
      indices.push_back(v.idx());

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glBindVertexArray(vao);
  // Allocate 16 vertices worth memory initially
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size() * 3, nullptr, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), nullptr, GL_STATIC_DRAW);
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

    glm::vec3 cameraPos = glm::vec3(std::cos(t), 1.0f, std::sin(t)) * 2.f;
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
  // if (key == GLFW_KEY_T && action == GLFW_PRESS)
  // {
  //   const size_t lastIx = vertices.size();
  //   for (size_t ix = 0; ix < 9; ++ix)
  //     vertices.push_back(vertices[lastIx - 9 + ix] + (ix % 3 == 2 ? 0.0f : 0.05f));
  //   for (int i = 0; i < 3; ++i)
  //     indices.push_back(static_cast<unsigned int>(indices.size()));

  //   // If allocated memory is enough, just use glBufferSubData for fast upload
  //   // When not enough, use glBufferData double memory size
  //   if (indices.size() <= numMaxVertices)
  //   {
  //     glBindBuffer(GL_ARRAY_BUFFER, vbo);
  //     glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertices.size(), vertices.data());
  //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  //     glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * indices.size(), indices.data());
  //     glBindBuffer(GL_ARRAY_BUFFER, 0);
  //   }
  //   else
  //   {
  //     // std::cout << "num vertices: " << indices.size() << ", max vertices: " << numMaxVertices << ". Amortize!\n";
  //     numMaxVertices *= 2;
  //     glBindBuffer(GL_ARRAY_BUFFER, vbo);
  //     glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numMaxVertices * 3, vertices.data(), GL_STATIC_DRAW);
  //     glBindBuffer(GL_ARRAY_BUFFER, 0);

  //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  //     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numMaxVertices, indices.data(), GL_STATIC_DRAW);
  //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  //   }
  // }
}

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, int w, int h)
{
  width = w;
  height = h;
  glViewport(0, 0, width, height);
}

void makeIcosahedronOMesh(MyMesh &oMesh)
{

  float phi = (1.0f + sqrt(5.0f)) * 0.5f; // golden ratio
  float a = 1.0f;
  float b = 1.0f / phi;

  std::vector<MyMesh::Point> points = {
      {0, b, -a},
      {b, a, 0},
      {-b, a, 0},
      {0, b, a},
      {0, -b, a},
      {-a, 0, b},
      {0, -b, -a},
      {a, 0, -b},
      {a, 0, b},
      {-a, 0, -b},
      {b, -a, 0},
      {-b, -a, 0},
  };

  std::vector<MyMesh::VertexHandle> vertices;
  for (const auto &p : points)
    vertices.push_back(oMesh.add_vertex(p / p.norm()));

  std::vector<std::vector<int>> foo = {{1, 2}, {3, 4}};

  std::vector<std::vector<MyMesh::VertexHandle>> faceTriangles = {
      {vertices[2], vertices[1], vertices[0]},   // 0
      {vertices[1], vertices[2], vertices[3]},   // 1
      {vertices[5], vertices[4], vertices[3]},   //
      {vertices[4], vertices[8], vertices[3]},   //
      {vertices[7], vertices[6], vertices[0]},   //
      {vertices[6], vertices[9], vertices[0]},   //
      {vertices[11], vertices[10], vertices[4]}, //
      {vertices[10], vertices[11], vertices[6]}, //
      {vertices[9], vertices[5], vertices[2]},   //
      {vertices[5], vertices[9], vertices[11]},  //
      {vertices[8], vertices[7], vertices[1]},   //
      {vertices[7], vertices[8], vertices[10]},  //
      {vertices[2], vertices[5], vertices[3]},   //
      {vertices[8], vertices[1], vertices[3]},   //
      {vertices[9], vertices[2], vertices[0]},   //
      {vertices[1], vertices[7], vertices[0]},   //
      {vertices[11], vertices[9], vertices[6]},  //
      {vertices[7], vertices[10], vertices[6]},  //
      {vertices[5], vertices[11], vertices[4]},  //
      {vertices[10], vertices[8], vertices[4]},  //
  };

  for (const auto &f : faceTriangles)
    oMesh.add_face(f);
}