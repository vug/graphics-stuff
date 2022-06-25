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
  glm::vec2 position_current{};
  glm::vec2 position_old{};
  std::function<glm::vec2(const glm::vec2 &)> potentialForce;
  float mass = 1.0f;
  float radius = 0.1f;

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
    // calculate forces
    const glm::vec2 acc = potentialForce(position_current) / mass;

    // apply boundry constraint
    const glm::vec2 center = {0, 0};
    const float border = 1.0f;
    const glm::vec2 relPos = position_current - center;
    const float dist = glm::length(relPos);
    if (dist > border - radius)
    {
      const glm::vec2 n = glm::normalize(relPos);
      position_current = center + n * (border - radius);

      // might need to update position_old if change in position_current is big.
      // const glm::vec2 acc2 = potentialForce(position_current) / mass;
      // position_old = position_current - .5f * acc2 * dt * dt;
    }

    // update position
    const glm::vec2 velocity = position_current - position_old;
    position_old = position_current;
    // position-Verlet
    position_current = position_current + velocity + acc * dt * dt;
  }
};

class Solver
{
public:
  std::vector<VerletObject> &objects;

  Solver(std::vector<VerletObject> &objects) : objects(objects) {}

  void update(float dt)
  {
    // collision constraint
    for (size_t i = 0; i < objects.size(); ++i)
    {
      for (size_t j = i + 1; j < objects.size(); ++j)
      {
        VerletObject &o1 = objects[i];
        VerletObject &o2 = objects[j];
        const glm::vec2 disp = o1.position_current - o2.position_current;
        const float dist = glm::length(disp);
        if (const float delta = dist - (o1.radius + o2.radius); delta < 0)
        {
          const glm::vec2 n = glm::normalize(disp);
          o1.position_current -= 0.5f * delta * n;
          o2.position_current += 0.5f * delta * n;
        }
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

  std::function<glm::vec2(const glm::vec2 &)> gravity = [](const glm::vec2 &)
  { return glm::vec2{0.0f, -1.0f}; };

  std::mt19937 rndGen;
  std::uniform_real_distribution<float> rndDist;

  Specs getSpecs() final
  {
    return {.name = "MyApp", .width = 800u, .height = 800u, .shouldDebugOpenGL = true};
  }

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

    // objects with which to start
    // objects.emplace_back(VerletObject{{0.5, 0.0}, gravity});
    // objects[0].radius = 0.2f;
    // objects.emplace_back(VerletObject{{-0.25, 0.0}, gravity});
    // objects[1].radius = 0.1f;
    solver = std::make_unique<Solver>(objects);

    mesh = std::make_unique<ws::Mesh>(objects.size());
    for (uint32_t ix = 0; const auto &obj : objects)
    {
      mesh->verts[ix] = ws::DefaultVertex{{obj.position_current.x, obj.position_current.y, 0}, {}, {}, {1, 1, 1, 1}, {obj.radius, 0, 0, 0}};
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

    // generate a new object at every period
    float period = 0.05f;
    static float remaining = period;
    static int maxBalls = 100;
    static float minRadius = 0.01f;
    static float maxRadius = 0.05f;
    remaining -= deltaTime;
    if (remaining < 0 && mesh->verts.size() < maxBalls)
    {
      objects.emplace_back(VerletObject{{0.0, 0.9}, gravity});
      auto &obj = objects[objects.size() - 1];
      obj.radius = (maxRadius - minRadius) * rndDist(rndGen) + minRadius;
      obj.position_old = obj.position_current + glm::vec2{std::sin(time) * 0.02, 0.02};
      remaining = period;

      mesh->verts.resize(objects.size());
      mesh->idxs.resize(objects.size());
    }

    solver->update(deltaTime);
    for (uint32_t ix = 0; const auto &obj : objects)
    {
      mesh->verts[ix] = ws::DefaultVertex{{obj.position_current.x, obj.position_current.y, 0}, {}, {}, {(ix % 256) / 256., (ix + 128) % 256 / 256., 1, 1}, {obj.radius, 0, 0, 0}};
      mesh->idxs[ix] = ix;
      ix++;
    }
    // when the number of objects is constant
    // for (size_t ix = 0; const auto &obj : objects)
    //   mesh->verts[ix++].position = {obj.position_current.x, obj.position_current.y, 0};

    // mesh->verts[0].position.y = std::sin(time) * 0.5f;
    // mesh->verts[0].position.x = std::cos(time) * 0.5f;
    mesh->uploadData();

    ImGui::Begin("Verlet Simulation");
    ImGui::Text("num balls: %d", mesh->verts.size());
    ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
    ImGui::SliderFloat("gen period", &period, 0.01f, 0.5f, "%.2f");
    ImGui::SliderInt("max balls", &maxBalls, 100, 1000);
    ImGui::SliderFloat("max radius", &maxRadius, 0.001f, 0.20f, "%.3f");
    if (maxRadius < minRadius)
      minRadius = maxRadius;
    ImGui::SliderFloat("min radius", &minRadius, 0.001f, 0.20f, "%.3f");
    if (minRadius > maxRadius)
      maxRadius = minRadius;
    ImGui::End();

    float rts[2] = {static_cast<float>(getSpecs().width), static_cast<float>(getSpecs().height)};

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