#include <App.h>
#include <Mesh.h>
#include <Shader.h>

#include <glad/gl.h>
#include <glm/vec2.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include <functional>
#include <memory>
#include <random>
#include <vector>

struct VerletObject
{
  glm::vec2 pos{};
  glm::vec2 vel{};
  float mass = 1.0f;
  float radius = 0.1f;
  glm::vec2 acc{};
};

using InterForce = std::function<glm::vec2(const glm::vec2 &p1, float m1, const glm::vec2 &p2, float m2)>;

class Solver
{
public:
  std::vector<VerletObject> &objects;
  InterForce interObjectForce;
  float period = 0.000016f; // .5 msec
  float potential{};
  float kinetic{};

private:
  float remaining{};

public:
  Solver(std::vector<VerletObject> &objects, InterForce interObjectForce)
      : objects(objects), interObjectForce(interObjectForce)
  {
    // calculate initial acc
    for (size_t i = 0; i < objects.size(); ++i)
    {
      for (size_t j = i + 1; j < objects.size(); ++j)
      {
        VerletObject &o1 = objects[i];
        VerletObject &o2 = objects[j];
        const glm::vec2 f = interObjectForce(o1.pos, o1.mass, o2.pos, o2.mass);
        o1.acc -= f / o1.mass;
        o2.acc += f / o2.mass;
      }
    }
  }

  void update(float dt)
  {
    remaining += dt;
    while (remaining > period)
    {
      // p[t + dt] = p[t] + v[t] dt + 1/2 a dt^2
      for (VerletObject &obj : objects)
        obj.pos = obj.pos + obj.vel * period + obj.acc * (period * period * 0.5f);

      // v[t + dt / 2] = v[t] + 1/2 a[t] dt
      for (VerletObject &obj : objects)
      {
        obj.vel = obj.vel + 0.5f * obj.acc * period;
        // after using acc reset it for the next computation/accumulation
        obj.acc = {};
      }

      // a[t + dt] = 1/m f(p[t + dt])
      for (size_t i = 0; i < objects.size(); ++i)
      {
        for (size_t j = i + 1; j < objects.size(); ++j)
        {
          VerletObject &o1 = objects[i];
          VerletObject &o2 = objects[j];
          const glm::vec2 f = interObjectForce(o1.pos, o1.mass, o2.pos, o2.mass);
          o1.acc -= f / o1.mass;
          o2.acc += f / o2.mass;
        }
      }

      // v[t + dt] = v[t + dt / 2] + 1/2 a[t + dt] dt
      for (VerletObject &obj : objects)
        obj.vel = obj.vel + 0.5f * obj.acc * period;

      remaining -= period;

      potential = 0.0f;
      kinetic = 0.0f;
      for (size_t i = 0; i < objects.size(); ++i)
      {
        VerletObject &o1 = objects[i];
        for (size_t j = i + 1; j < objects.size(); ++j)
        {
          VerletObject &o2 = objects[j];
          potential += -o1.mass * o2.mass / glm::length(o1.pos - o2.pos);
        }
        kinetic += 0.5f * o1.mass * glm::dot(o1.vel, o1.vel);
      }
    }
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

  InterForce gravity = [](const glm::vec2 &p1, float m1, const glm::vec2 &p2, float m2)
  {
    const float tolarence = 0.00001f;
    glm::vec2 r = p1 - p2;
    return 0.01f * m1 * m2 * glm::normalize(r) / std::max(glm::dot(r, r), tolarence);
  };

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
  gl_Position = ProjectionFromView * vec4(vPos, 1.0);
  // gl_Position = vec4(vPos, 1.0);
  float radius = vCustom.x;
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

    // objects.emplace_back(VerletObject{{0, 0}, {0, 0}});
    // objects[0].mass = 10.0f;
    for (int n = 0; n < 40; n++)
    {
      float x = 2.0f * rndDist(rndGen) - 1.0f;
      float y = 2.0f * rndDist(rndGen) - 1.0f;
      glm::vec2 p = {x, y};
      p = (p * 0.40f) + (glm::normalize(p) * 0.05f);

      // const float nx = (2.0f * rndDist(rndGen) - 1.0f) * 0.0000000001f;
      // const float ny = (2.0f * rndDist(rndGen) - 1.0f) * 0.0000000001f;
      // glm::vec2 v = {nx, ny};

      // glm::vec2 v = {0, 0};

      glm::vec2 v = glm::vec2{-y, x} * 0.5f;
      objects.emplace_back(VerletObject{p, v, 1.5f, 0.01f});
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
    deltaTime *= 0.1f;
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    solver->update(deltaTime);
    // objects[0].pos = {0, 0};
    // when the number of objects is constant
    for (size_t ix = 0; const auto &obj : objects)
      mesh->verts[ix++].position = {obj.pos.x, obj.pos.y, 0};

    mesh->uploadData();

    ImGui::Begin("Verlet Simulation");
    ImGui::Text("Frame dur: %.4f, FPS: %.1f", deltaTime, 1.0f / deltaTime);
    ImGui::InputFloat("Solver period", &solver->period, 0.001f, 0, "%.4f", ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::Text("Potential: %+3.2e, Kinetic: %+3.2e, Total: %+3.2e", solver->potential, solver->kinetic, solver->potential + solver->kinetic);
    static float areaSize = 1.0f;
    ImGui::SliderFloat("Area Size", &areaSize, 0.1f, 100.f, "%3.1f");
    ImGui::End();

    float rts[2] = {static_cast<float>(width), static_cast<float>(height)};
    const glm::mat4 projOrtho = glm::ortho(-areaSize, areaSize, -areaSize, areaSize, -1.f, 1.f);

    glUseProgram(quadShader->getId());
    quadShader->setVector2fv("RenderTargetSize", rts);
    backgroundMesh->uploadData();
    glBindVertexArray(backgroundMesh->vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(backgroundMesh->idxs.size()), GL_UNSIGNED_INT, 0);

    glUseProgram(pointShader->getId());
    pointShader->setVector2fv("RenderTargetSize", rts);
    pointShader->setMatrix4fv("ProjectionFromView", glm::value_ptr(projOrtho));
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