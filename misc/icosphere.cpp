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
#include <OpenMesh/Tools/Subdivider/Uniform/LoopT.hh>

#include <iostream>
#include <vector>

using MyMesh = OpenMesh::TriMesh_ArrayKernelT<>;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void makeIcosahedronOMesh(MyMesh &oMesh);
void makeIcosphereOMesh(MyMesh &oMesh, uint32_t numSubDiv);

const char *vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VertexData
{
  vec3 modelPos;
  vec3 modelNorm;
} vertexData;

void main()
{
    gl_Position = projection * view * model * vec4(vPos, 1.0);
    vertexData.modelPos = vPos;
    vertexData.modelNorm = vNorm;
}
)";
const char *fragmentShaderSource = R"(
#version 460 core

in VertexData
{
  vec3 modelPos;
  vec3 modelNorm;
} vertexData;

out vec4 FragColor;
void main()
{
    // FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    FragColor = vec4(vertexData.modelNorm, 1.0f);
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
  // makeIcosahedronOMesh(oMesh);
  makeIcosphereOMesh(oMesh, 3);

  std::vector<glm::vec3> positions = {};
  std::vector<glm::vec3> normals = {};
  std::vector<unsigned int> indices = {};
  unsigned int vboPos, vboNorm, vao, ebo;

  for (const auto &v : oMesh.vertices())
  {
    const auto &p = oMesh.point(v);
    positions.emplace_back(p[0], p[1], p[2]);
  }

  normals.resize(positions.size());
  for (auto f : oMesh.faces())
  {
    // const auto& fNorm = oMesh.calc_normal(f);
    for (auto v : f.vertices())
    {
      const int ix = v.idx();
      indices.push_back(v.idx());

      // flat shading for normals: won't work because vertices are shared among faces
      // glm::vec3 n = {fNorm[0], fNorm[1], fNorm[2]};

      const auto &vNorm = oMesh.calc_normal(v);
      glm::vec3 n = {vNorm[0], vNorm[1], vNorm[2]};

      // cheat for sphere
      // const auto& p = oMesh.point(v);
      // glm::vec3 n = {p[0], p[1], p[2]};

      n += 1.f;
      n *= 0.5f;
      normals[ix] = n;
    }
  }
  std::cout << positions.size() << " " << normals.size() << " " << indices.size() << "\n";

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vboPos);
  glGenBuffers(1, &vboNorm);
  glGenBuffers(1, &ebo);
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vboPos);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size() * 3, positions.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  glBindBuffer(GL_ARRAY_BUFFER, vboNorm);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size() * 3, normals.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);

  while (!glfwWindowShouldClose(window))
  {
    double t = glfwGetTime();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
  glDeleteBuffers(1, &vboPos);
  glDeleteBuffers(1, &vboNorm);
  glDeleteBuffers(1, &ebo);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}

void key_callback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mode)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
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

void makeIcosphereOMesh(MyMesh &oMesh, uint32_t numSubDiv)
{
  makeIcosahedronOMesh(oMesh);

  if (numSubDiv == 0)
    return;

  OpenMesh::Subdivider::Uniform::LoopT<MyMesh> loopSubd;
  loopSubd.attach(oMesh);
  loopSubd(numSubDiv);
  loopSubd.detach();

  for (auto &v : oMesh.vertices())
  {
    auto &p = oMesh.point(v);
    p.normalize();
  }
}