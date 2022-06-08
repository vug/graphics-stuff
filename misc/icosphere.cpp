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
// logic from https://www.danielsieger.com/blog/2021/03/27/generating-spheres.html
void makeIcosahedronOMesh(MyMesh &oMesh);
void makeIcosphereOMesh(MyMesh &oMesh, uint32_t numSubDiv);
void makeIndexedMeshFromOMesh(const MyMesh &oMesh, std::vector<glm::vec3> &positions, std::vector<glm::vec3> &normals, std::vector<uint32_t> &indices);
void makeTriangleMeshFromOMesh(const MyMesh &oMesh, std::vector<glm::vec3> &positions, std::vector<glm::vec3> &normals, std::vector<uint32_t> &indices, bool flat = true);

struct Light
{
  glm::vec3 position;
  float padding1;
  glm::vec3 color;
  float padding2;
};
struct Lights
{
  std::vector<Light> lights;
  uint32_t numLights;
};

const char *vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;

uniform mat4 WorldFromObject;
uniform mat3 WorldNormalFromObject;
uniform mat4 ViewFromWorld;
uniform mat4 ProjectionFromView;

out VertexData
{
  vec3 ObjectPosition;
  vec3 WorldPosition;
  vec3 ObjectNormal;
  vec3 WorldNormal;
} vertexData;

void main()
{
    gl_Position = ProjectionFromView * ViewFromWorld * WorldFromObject * vec4(vPos, 1.0);

    vertexData.ObjectPosition = vPos;
    vertexData.WorldPosition = vec3(WorldFromObject * vec4(vertexData.ObjectPosition, 1.0));
    vertexData.ObjectNormal = vNorm;
    // vertexData.WorldNormal = WorldNormalFromObject * vNorm;
    vertexData.WorldNormal = mat3(transpose(inverse(WorldFromObject))) * vNorm;
}
)";
const char *fragmentShaderSource = R"(
#version 460 core

uniform vec3 cameraPos;

struct Light
{
  vec3 position;
  float padding1;
  vec3 color;
  float padding2;
};

layout (std140, binding = 0) uniform Lights
{
  Light light[100];
  int numLights;
};

in VertexData
{
  vec3 ObjectPosition;
  vec3 WorldPosition;
  vec3 ObjectNormal;
  vec3 WorldNormal;
} vertexData;

out vec4 FragColor;
void main()
{
    // FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    // FragColor = vec4((vertexData.ObjectNormal * 0.5 + 0.5), 1.0f);
    // FragColor = vec4((normalize(vertexData.WorldNormal) * 0.5 + 0.5), 1.0f);

    vec3 objectColor = vec3(0.1, 0.2, 0.8);
    float ambientStrength = 0.1;
    float diffuseStrength = 0.75;
    float specularStrength = 2.5;

    vec3 result = vec3(0.0);
    for (int i = 0; i < numLights; i++)
    {
      // ambient
      vec3 ambient = ambientStrength * light[i].color;
      
      // diffuse 
      vec3 norm = normalize(vertexData.WorldNormal);
      vec3 lightDir = normalize(light[i].position - vertexData.WorldPosition);
      float diff = max(dot(norm, lightDir), 0.0);
      vec3 diffuse = diffuseStrength * diff * light[i].color;
      
      // specular
      vec3 viewDir = normalize(cameraPos - vertexData.WorldPosition);
      vec3 reflectDir = reflect(-lightDir, norm);  
      float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
      vec3 specular = specularStrength * spec * light[i].color;  
          
      result += (ambient + diffuse + specular) * objectColor;
    }

    // silly toon shading
    // int nShades = 5;
    // result.rgb = floor(result.rgb * nShades) / nShades;

    FragColor = vec4(result, 1.0);
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
  makeIcosphereOMesh(oMesh, 0);

  std::vector<glm::vec3> positions = {};
  std::vector<glm::vec3> normals = {};
  std::vector<unsigned int> indices = {};

  // makeIndexedMeshFromOMesh(oMesh, positions, normals, indices);
  makeTriangleMeshFromOMesh(oMesh, positions, normals, indices);

  unsigned int vboPos, vboNorm, vao, ebo, uboLights;
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

  Lights lights;
  lights.lights = {
      {{4.f, 0.f, 0.f}, 0, {1.0f, 0.0f, 0.0f}, 0},
      {{0.f, 4.f, 0.f}, 0, {0.0f, 1.0f, 0.0f}, 0},
      {{0.f, 0.f, 4.f}, 0, {0.0f, 0.0f, 1.0f}, 0},
  };
  lights.numLights = static_cast<uint32_t>(lights.lights.size());
  lights.lights.resize(100);

  unsigned int lightsIdx = glGetUniformBlockIndex(shaderProgram, "Lights");
  glUniformBlockBinding(shaderProgram, lightsIdx, 0);

  glGenBuffers(1, &uboLights);
  glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
  size_t sizeofLights = sizeof(Light) * 100 + sizeof(int);
  glBufferData(GL_UNIFORM_BUFFER, sizeofLights, nullptr, GL_DYNAMIC_DRAW); // this allocates space for the UBO.
  glBindBufferRange(GL_UNIFORM_BUFFER, lightsIdx, uboLights, 0, sizeofLights);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  // glDisable(GL_CULL_FACE);
  // glCullFace(GL_FRONT);

  while (!glfwWindowShouldClose(window))
  {
    float t = static_cast<float>(glfwGetTime());

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    const float fov = 20.0f;

    // glm::vec3 cameraPos = glm::vec3(std::cos(t), 0.2, std::sin(t)) * 5.f;
    glm::vec3 cameraPos = glm::vec3(0.f, 0.2f, 5.f);
    const int cameraPosLoc = glGetUniformLocation(shaderProgram, "cameraPos");
    glUniform3fv(cameraPosLoc, 1, glm::value_ptr(cameraPos));

    glm::mat4 ViewFromWorld = glm::lookAt(cameraPos, cameraTarget, up);
    const int ViewFromWorldLoc = glGetUniformLocation(shaderProgram, "ViewFromWorld");
    glUniformMatrix4fv(ViewFromWorldLoc, 1, GL_FALSE, glm::value_ptr(ViewFromWorld));

    glm::mat4 ProjectionFromView = glm::perspective(glm::radians(fov), static_cast<float>(width) / height, 0.1f, 100.0f);
    const int ProjectionFromViewLoc = glGetUniformLocation(shaderProgram, "ProjectionFromView");
    glUniformMatrix4fv(ProjectionFromViewLoc, 1, GL_FALSE, glm::value_ptr(ProjectionFromView));

    glUseProgram(shaderProgram);

    glBindVertexArray(vao);
    glm::mat4 WorldFromObject = glm::rotate(glm::mat4(1.0f), t, {0.f, 1.f, 0.f});
    WorldFromObject = glm::scale(WorldFromObject, {0.3, 0.5, 0.1});
    const int WorldFromObjectLoc = glGetUniformLocation(shaderProgram, "WorldFromObject");
    glUniformMatrix4fv(WorldFromObjectLoc, 1, GL_FALSE, glm::value_ptr(WorldFromObject));

    glm::mat4 WorldNormalFromObject = glm::mat3(glm::transpose(glm::inverse(WorldFromObject)));
    const int WorldNormalFromObjectLoc = glGetUniformLocation(shaderProgram, "WorldNormalFromObject");
    glUniformMatrix3fv(WorldNormalFromObjectLoc, 1, GL_FALSE, glm::value_ptr(WorldNormalFromObject));

    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    // glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeofLights, &lights); // crashes :-O
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Light) * 100, lights.lights.data());
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Light) * 100, sizeof(int32_t), &lights.numLights);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(positions.size()));
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

  // At each iteration first subdivide then project on sphere instead of subdividing n-times and projecting at the end
  for (uint32_t i = 0; i < numSubDiv; ++i)
  {
    OpenMesh::Subdivider::Uniform::LoopT<MyMesh> loopSubd;
    loopSubd.attach(oMesh);
    loopSubd(1);
    loopSubd.detach();

    for (auto &v : oMesh.vertices())
    {
      auto &p = oMesh.point(v);
      p.normalize();
    }
  }
}

void makeIndexedMeshFromOMesh(const MyMesh &oMesh, std::vector<glm::vec3> &positions, std::vector<glm::vec3> &normals, std::vector<uint32_t> &indices)
{
  positions.clear();
  normals.clear();
  indices.clear();

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

      normals[ix] = n;
    }
  }
}

void makeTriangleMeshFromOMesh(const MyMesh &oMesh, std::vector<glm::vec3> &positions, std::vector<glm::vec3> &normals, std::vector<uint32_t> &indices, bool flat)
{
  positions.clear();
  normals.clear();
  indices.clear();

  normals.resize(positions.size());
  for (auto f : oMesh.faces())
  {
    const auto &fNorm = oMesh.calc_normal(f);
    for (auto v : f.vertices())
    {
      const auto &p = oMesh.point(v);
      positions.emplace_back(p[0], p[1], p[2]);

      if (flat)
        normals.emplace_back(fNorm[0], fNorm[1], fNorm[2]);
      else
      {
        const auto &vNorm = oMesh.calc_normal(v);
        normals.emplace_back(vNorm[0], vNorm[1], vNorm[2]);
      }
    }
  }
}