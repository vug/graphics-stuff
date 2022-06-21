#include <App.h>
#include <Mesh.h>
#include <Shader.h>

#include <glad/gl.h>
#include <glm/vec2.hpp>

#include <functional>
#include <memory>
#include <vector>

struct VerletObject
{
  glm::vec2 position_current{};
  glm::vec2 position_old{};
  std::function<glm::vec2(const glm::vec2 &)> potentialForce;
  bool first = true;
  float mass = 1.0f;

  VerletObject(glm::vec2 pos, std::function<glm::vec2(const glm::vec2 &)> potentialForce)
      : position_current(pos), potentialForce(potentialForce)
  {
    const float dt = 1.f / 60;
    position_current = pos;
    const glm::vec2 acc = potentialForce(position_current) / mass;
    position_old = position_current - .5f * acc * dt * dt;
  }

  void updatePosition(float dt)
  {
    const glm::vec2 velocity = position_current - position_old;
    // Verlet
    const glm::vec2 acc = potentialForce(position_current) / mass;
    position_current = position_current + velocity + acc + dt * dt;

    position_old = position_current;
  }
};

class Solver
{
public:
  std::vector<VerletObject> &objects;

  Solver(std::vector<VerletObject> &objects) : objects(objects) {}

  void update(float dt)
  {
    for (VerletObject &obj : objects)
      obj.updatePosition(dt);
  }
};

class MyApp : public ws::App
{
public:
  std::vector<VerletObject> objects;
  std::unique_ptr<Solver> solver;

  std::unique_ptr<ws::Shader> mainShader;
  std::unique_ptr<ws::Shader> pointShader;
  std::unique_ptr<ws::Mesh> mesh;

  Specs getSpecs() final
  {
    return {.name = "MyApp", .width = 800u, .height = 600u, .shouldDebugOpenGL = true};
  }

  void onInit() final
  {
    const char *mainShaderVertex = R"(
#version 460 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec2 vUV;
layout (location = 3) in vec4 vColor;

uniform mat4 WorldFromObject;
uniform mat4 ViewFromWorld;
uniform mat4 ProjectionFromView;

out VertexData
{
  vec3 position;
  vec3 normal;
  vec2 uv;
  vec4 color;
} vertexData;

void main()
{
  //gl_Position = ProjectionFromView * ViewFromWorld * WorldFromObject * vec4(vPos, 1.0);
  gl_Position = vec4(vPos, 1.0);
  gl_PointSize = 20.0f;

  vertexData.position = vPos;
  vertexData.normal = vNorm;
  vertexData.uv = vUV;
  vertexData.color = vColor;
}
)";

    const char *pointShaderFragment = R"(
#version 460 core

in VertexData
{
  vec3 position;
  vec3 normal;
  vec2 uv;
  vec4 color;
} vertexData;

out vec4 FragColor;

void main()
{
  FragColor = vertexData.color;
}
)";
    // mainShader = std::make_unique<ws::Shader>(mainShaderVertex, mainShaderFragment);
    pointShader = std::make_unique<ws::Shader>(mainShaderVertex, pointShaderFragment);

    auto gravity = [](const glm::vec2 &)
    { return glm::vec2{0.0f, -0.01f}; };
    objects.emplace_back(VerletObject{{}, gravity});
    solver = std::make_unique<Solver>(objects);

    mesh = std::make_unique<ws::Mesh>(objects.size());
    for (uint32_t ix = 0; const auto &obj : objects)
    {
      mesh->verts[ix] = ws::DefaultVertex{{obj.position_current.x, obj.position_current.y, 0}};
      mesh->idxs[ix] = ix;
      ix++;
    }
    mesh->uploadData();

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    // glEnable(GL_CULL_FACE);
    // glEnable(GL_DEPTH_TEST);
  }

  void onRender(float time, float deltaTime) final
  {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    solver->update(deltaTime);
    for (size_t ix = 0; const auto &obj : objects)
      mesh->verts[ix++].position = {obj.position_current.x, obj.position_current.y, 0};
    // mesh->verts[0].position.y = std::sin(time) * 0.5f;
    // mesh->verts[0].position.x = std::cos(time) * 0.5f;
    mesh->uploadData();

    glUseProgram(pointShader->id);
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_POINTS, static_cast<GLsizei>(mesh->idxs.size()), GL_UNSIGNED_INT, 0);
  }

  void onDeinit() final
  {
  }
};

int main()
{
  MyApp app;
  app.run();
  return 0;
}