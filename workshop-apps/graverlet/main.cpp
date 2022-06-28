#include <App.h>
#include <Mesh.h>
#include <Shader.h>

#include <glad/gl.h>
#include <glm/vec2.hpp>
#include <glm/geometric.hpp>
#include <imgui.h>

#include <functional>
#include <memory>
#include <random>
#include <vector>

struct VerletObject
{
  glm::vec2 pos{};
  glm::vec2 position_old{};
  glm::vec2 totalForce{};
  float mass = 1.0f;
  float radius = 0.1f;

  void updatePosition(float dt)
  {
    const glm::vec2 acc = totalForce / mass;

    // update position
    const glm::vec2 velocity = pos - position_old;
    position_old = pos;
    // position-Verlet
    pos = pos + velocity + acc * dt * dt;

    totalForce = {};
  }
};

class Solver
{
public:
  std::vector<VerletObject> &objects;
  std::function<glm::vec2(const VerletObject &, const VerletObject &)> interObjectForce;

  Solver(std::vector<VerletObject> &objects, std::function<glm::vec2(const VerletObject &, const VerletObject &)> interObjectForce)
      : objects(objects), interObjectForce(interObjectForce) {}

  void update(float dt)
  {
    // calculate forces
    for (size_t i = 0; i < objects.size(); ++i)
    {
      for (size_t j = i + 1; j < objects.size(); ++j)
      {
        VerletObject &o1 = objects[i];
        VerletObject &o2 = objects[j];
        const glm::vec2 f = interObjectForce(o1, o2);
        o1.totalForce -= f;
        o2.totalForce += f;
      }
    }

    for (VerletObject &obj : objects)
      obj.updatePosition(dt);
  }
};

class MyApp : public ws::App
{
public:
  std::vector<VerletObject> objects;
  std::unique_ptr<Solver> solver;

  std::unique_ptr<ws::Shader> quadShader;
  std::unique_ptr<ws::Shader> pointShader;
  std::unique_ptr<ws::Mesh> mesh;
  std::unique_ptr<ws::Mesh> backgroundMesh;

  std::function<glm::vec2(const VerletObject &, const VerletObject &)> gravity = [](const VerletObject &obj1, const VerletObject &obj2)
  { glm::vec2 r = obj1.pos - obj2.pos; return obj1.mass * obj2.mass * glm::normalize(r) / glm::dot(r, r); };

  std::mt19937 rndGen;
  std::uniform_real_distribution<float> rndDist;

  MyApp() : App({.name = "MyApp", .width = 800u, .height = 800u, .shouldDebugOpenGL = true}) {}

  void onInit() final
  {
    const char *mainShaderVertex = R"(
#version 460 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec2 vUV;
layout (location = 3) in vec4 vColor;
layout (location = 4) in vec4 vCustom;

uniform mat4 WorldFromObject;
uniform mat4 ViewFromWorld;
uniform mat4 ProjectionFromView;
uniform vec2 RenderTargetSize;

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
  float radius = vCustom.x;
  gl_Position = vec4(vPos, 1.0);
  gl_PointSize = radius * RenderTargetSize.y;

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
  vec2 p = 2 * gl_PointCoord - 1;
  if (dot(p, p) > 1)
    discard;
  FragColor = vertexData.color;
}
)";

    const char *diskShaderFragment = R"(
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
  vec2 p = 2 * vertexData.uv - 1;
  if (dot(p, p) > 1)
    discard;
  FragColor = vertexData.color;
}
)";
    pointShader = std::make_unique<ws::Shader>(mainShaderVertex, pointShaderFragment);
    quadShader = std::make_unique<ws::Shader>(mainShaderVertex, diskShaderFragment);

    objects.emplace_back(VerletObject{{0, 0}, {0, 0}});
    objects[0].mass = 10.0f;
    for (int n = 0; n < 40; n++)
    {
      float x = 2.0f * rndDist(rndGen) - 1.0f;
      float y = 2.0f * rndDist(rndGen) - 1.0f;
      glm::vec2 v = {x, y};
      v = (v * 0.40f) + (glm::normalize(v) * 0.15f);
      const glm::vec2 v0 = {v.x - 0.015f * v.y, v.y + 0.015f * v.x};
      objects.emplace_back(VerletObject{v, v0, {}, 0.015f, 0.02f});
      // const float nx = (2.0f * rndDist(rndGen) - 1.0f) * 0.01f;
      // const float ny = (2.0f * rndDist(rndGen) - 1.0f) * 0.01f;
      // objects.emplace_back(VerletObject{{x, y}, {x + nx, y + ny}, {}, 0.01f, 0.02f});
    }

    // // objects with which to start
    // objects.emplace_back(VerletObject{{0.5, 0.0}, {0.5, 0.005}});
    // // objects[0].radius = 0.2f;
    // objects.emplace_back(VerletObject{{-0.5, -0.1}, {-0.5, -0.105}});
    // // objects[1].radius = 0.1f;
    solver = std::make_unique<Solver>(objects, gravity);

    mesh = std::make_unique<ws::Mesh>(objects.size());
    for (uint32_t ix = 0; const auto &obj : objects)
    {
      mesh->verts[ix] = ws::DefaultVertex{{obj.pos.x, obj.pos.y, 0}, {}, {}, {1, 1, 1, 1}, {obj.radius, 0, 0, 0}};
      mesh->idxs[ix] = ix;
      ix++;
    }
    mesh->uploadData();

    backgroundMesh.reset(new ws::Mesh(ws::Mesh::makeQuad())); // does not call Mesh destructor
    // backgroundMesh = std::make_unique<ws::Mesh>(ws::Mesh::makeQuad()); // calls Mesh destructor -> glDeletes buffers
    for (auto &v : backgroundMesh->verts)
    {
      v.color = {0.1, 0.1, 0.1, 1.};
      v.position.z = 0.1f;
    }
    backgroundMesh->uploadData();

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    // glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
  }

  void onRender([[maybe_unused]] float time, [[maybe_unused]] float deltaTime) final
  {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    solver->update(deltaTime * 0.1f);
    objects[0].pos = objects[0].position_old = {0, 0};
    // when the number of objects is constant
    for (size_t ix = 0; const auto &obj : objects)
      mesh->verts[ix++].position = {obj.pos.x, obj.pos.y, 0};

    // mesh->verts[0].position.y = std::sin(time) * 0.5f;
    // mesh->verts[0].position.x = std::cos(time) * 0.5f;
    mesh->uploadData();

    ImGui::Begin("Verlet Simulation");
    ImGui::Text("Frame dur: %.4f, FPS: %.1f", deltaTime, 1.0f / deltaTime);
    ImGui::End();

    float rts[2] = {static_cast<float>(width), static_cast<float>(height)};

    glUseProgram(quadShader->getId());
    quadShader->setVector2fv("RenderTargetSize", rts);
    backgroundMesh->uploadData();
    glBindVertexArray(backgroundMesh->vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(backgroundMesh->idxs.size()), GL_UNSIGNED_INT, 0);

    glUseProgram(pointShader->getId());
    pointShader->setVector2fv("RenderTargetSize", rts);
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