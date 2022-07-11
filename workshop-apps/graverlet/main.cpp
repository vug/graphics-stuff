#include <App.h>
#include <Mesh.h>
#include <Shader.h>

#include <glad/gl.h>
#include <glm/vec2.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <implot.h>

#include <algorithm>
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

const float G = 0.2f;
const float softening = 1.0f;
using InterForce = std::function<glm::vec2(const VerletObject &obj1, const VerletObject &obj2)>;

class Solver
{
public:
  std::vector<VerletObject> &objects;
  InterForce interObjectForce;
  float potential{};
  float kinetic{};

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
        const glm::vec2 f = interObjectForce(o1, o2);
        o1.acc -= f / o1.mass;
        o2.acc += f / o2.mass;
      }
    }
  }

  void update(float period, int numIter)
  {

    for (int n = 0; n < numIter; ++n)
    {
      // p[t + dt] = p[t] + v[t] dt + 1/2 a dt^2
      for (VerletObject &obj : objects)
        obj.pos += obj.vel * period + obj.acc * (period * period * 0.5f);

      // v[t + dt / 2] = v[t] + 1/2 a[t] dt
      for (VerletObject &obj : objects)
      {
        obj.vel += 0.5f * obj.acc * period;
        // after using acc reset it for the next computation/accumulation
        obj.acc = {};
      }

      // a[t + dt] = 1/m f(p[t + dt])
      for (size_t i = 0; i < objects.size(); ++i)
      {
        for (size_t j = 0; j < objects.size(); ++j)
        {
          VerletObject &o1 = objects[i];
          VerletObject &o2 = objects[j];
          o1.acc -= interObjectForce(o1, o2) / o1.mass; // acc
        }
      }

      // v[t + dt] = v[t + dt / 2] + 1/2 a[t + dt] dt
      for (VerletObject &obj : objects)
        obj.vel += 0.5f * obj.acc * period;

      potential = 0.0f;
      kinetic = 0.0f;
      for (size_t i = 0; i < objects.size(); ++i)
      {
        VerletObject &o1 = objects[i];
        for (size_t j = i + 1; j < objects.size(); ++j)
        {
          VerletObject &o2 = objects[j];
          glm::vec2 r = o1.pos - o2.pos;
          const float r2 = glm::dot(r, r);
          potential -= G * o1.mass * o2.mass / glm::pow(r2 + softening, 0.5f);
        }
        kinetic += 0.5f * o1.mass * glm::dot(o1.vel, o1.vel);
      }
    }
  }
};

class PlotBuffer
{
private:
  size_t maxSize = 1024;
  size_t offset = 0;
  std::vector<glm::vec2> data;

public:
  PlotBuffer()
  {
    data.reserve(maxSize);
  }

  PlotBuffer(size_t maxSize) : maxSize(maxSize)
  {
    data.reserve(maxSize);
  }

  void addPoint(const glm::vec2 &p)
  {
    if (data.size() < maxSize)
      data.push_back(p);
    else
    {
      data[offset] = p;
      offset = (offset + 1) % maxSize;
    }
  }

  const std::vector<glm::vec2> &getData() const
  {
    return data;
  }

  const size_t &getOffset() const
  {
    return offset;
  }
};

class EnergiesPlot
{
private:
  PlotBuffer potentials;
  PlotBuffer kinetics;
  PlotBuffer totals;
  float yMin{}, yMax{};
  float tMin{}, tMax{};

public:
  EnergiesPlot(size_t numPoints)
      : potentials(numPoints), kinetics(numPoints), totals(numPoints) {}
  void
  addEnergyPoints(float time, float potential, float kinetic, float total)
  {
    potentials.addPoint({time, potential});
    kinetics.addPoint({time, kinetic});
    totals.addPoint({time, total});

    const bool allTime = false;
    if (allTime)
    {
      yMin = std::min(std::min(std::min(yMin, potential), kinetic), total);
      yMax = std::max(std::max(std::max(yMax, potential), kinetic), total);
    }
    else
    {
      static const auto comp = [](const glm::vec2 &a, const glm::vec2 &b)
      { return a.y < b.y; };
      const auto [potMin, potMax] = std::minmax_element(potentials.getData().begin(), potentials.getData().end(), comp);
      const auto [kinMin, kinMax] = std::minmax_element(kinetics.getData().begin(), kinetics.getData().end(), comp);
      const auto [totMin, totMax] = std::minmax_element(totals.getData().begin(), totals.getData().end(), comp);
      yMin = std::min(std::min(potMin->y, kinMin->y), totMin->y);
      yMax = std::max(std::max(potMax->y, kinMax->y), totMax->y);
    }

    {
      static const auto comp = [](const glm::vec2 &a, const glm::vec2 &b)
      { return a.x < b.x; };
      const auto [mn, mx] = std::minmax_element(potentials.getData().begin(), potentials.getData().end(), comp);
      tMin = mn->x;
      tMax = mx->x;
    }
  }

  void plot(const ImVec2 &size)
  {
    if (ImPlot::BeginPlot("Energies", size))
    {
      const auto &pData = potentials.getData();
      const auto &kData = kinetics.getData();
      const auto &tData = totals.getData();
      ImPlot::SetupAxes("time", "energy", ImPlotAxisFlags_None, ImPlotAxisFlags_None);

      ImPlot::SetupAxis(ImAxis_X1, "time", ImPlotAxisFlags_AuxDefault);
      ImPlot::SetupAxisLimits(ImAxis_X1, tMin, tMax, ImGuiCond_Always);
      ImPlot::SetupAxisLimits(ImAxis_Y1, yMin, yMax, ImGuiCond_Always);

      ImPlot::PlotLine("Potential", &pData[0].x, &pData[0].y, static_cast<int>(pData.size()), 0, static_cast<int>(potentials.getOffset()), 2 * sizeof(float));
      ImPlot::PlotLine("Kinetic", &kData[0].x, &kData[0].y, static_cast<int>(kData.size()), 0, static_cast<int>(kinetics.getOffset()), 2 * sizeof(float));
      ImPlot::PlotLine("Total", &tData[0].x, &tData[0].y, static_cast<int>(tData.size()), 0, static_cast<int>(totals.getOffset()), 2 * sizeof(float));
      ImPlot::EndPlot();
    }
  }
};

// clang-format off
// https://stackoverflow.com/questions/21977786/star-b-v-color-index-to-apparent-rgb-color
glm::vec4 bv2rgb(double bv)    // RGB <0,1> <- BV <-0.4,+2.0> [-]
{
  double t; double r=0.0, g=0.0, b=0.0; 
  if       (bv< -0.40) bv=-0.4; 
  if       (bv>  2.00) bv= 2.0;
  if      ((bv>=-0.40)&&(bv<0.00)) { t=(bv+0.40)/(0.00+0.40); r=0.61+(0.11*t)+(0.1*t*t); }
  else if ((bv>= 0.00)&&(bv<0.40)) { t=(bv-0.00)/(0.40-0.00); r=0.83+(0.17*t)          ; }
  else if ((bv>= 0.40)&&(bv<2.10)) { t=(bv-0.40)/(2.10-0.40); r=1.00                   ; }
       if ((bv>=-0.40)&&(bv<0.00)) { t=(bv+0.40)/(0.00+0.40); g=0.70+(0.07*t)+(0.1*t*t); }
  else if ((bv>= 0.00)&&(bv<0.40)) { t=(bv-0.00)/(0.40-0.00); g=0.87+(0.11*t)          ; }
  else if ((bv>= 0.40)&&(bv<1.60)) { t=(bv-0.40)/(1.60-0.40); g=0.98-(0.16*t)          ; }
  else if ((bv>= 1.60)&&(bv<2.00)) { t=(bv-1.60)/(2.00-1.60); g=0.82         -(0.5*t*t); }
       if ((bv>=-0.40)&&(bv<0.40)) { t=(bv+0.40)/(0.40+0.40); b=1.00                   ; }
  else if ((bv>= 0.40)&&(bv<1.50)) { t=(bv-0.40)/(1.50-0.40); b=1.00-(0.47*t)+(0.1*t*t); }
  else if ((bv>= 1.50)&&(bv<1.94)) { t=(bv-1.50)/(1.94-1.50); b=0.63         -(0.6*t*t); }
  return {r, g, b, 1};
}
// clang-format on

class MyApp : public ws::App
{
public:
  std::vector<VerletObject> objects;
  std::unique_ptr<Solver> solver;

  std::unique_ptr<ws::Shader> pointShader;
  std::unique_ptr<ws::Mesh> mesh;

  InterForce gravity = [](const VerletObject &obj1, const VerletObject &obj2)
  {
    glm::vec2 r = obj1.pos - obj2.pos;
    const float r2 = glm::dot(r, r);
    return G * obj1.mass * obj2.mass * r / glm::pow(r2 + softening, 1.5f);
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
  float alpha = (1.0 - smoothstep(0.25, 1.0, length(p))) * 0.1;
  FragColor = vec4(vertexData.color.rgb, alpha);
}
)";

    pointShader = std::make_unique<ws::Shader>(mainShaderVertex, pointShaderFragment);

    for (int n = 0; n < 500; n++)
    {
      const float theta = 2.0f * 3.14159265f * rndDist(rndGen);
      const float r = 20.0f * rndDist(rndGen);

      const float x = r * std::cos(theta);
      const float y = r * std::sin(theta);
      glm::vec2 p = {x, y};
      p = (p * 0.40f) + (glm::normalize(p) * 0.05f);

      glm::vec2 v = glm::vec2{-y, x} * (20.0f - r) / 20.0f * 1.0f;
      objects.emplace_back(VerletObject{p, v, 1.5f, 0.04f});
    }
    solver = std::make_unique<Solver>(objects, gravity);

    mesh = std::make_unique<ws::Mesh>(objects.size());
    for (uint32_t ix = 0; const auto &obj : objects)
    {
      // TODO: learn and use actual star bv distribution instead of uniform dist
      const glm::vec4 color = bv2rgb(rndDist(rndGen) * 2.4f - 0.4f);
      mesh->verts[ix] = ws::DefaultVertex{{obj.pos.x, obj.pos.y, 0}, {}, {}, color, {obj.radius, 0, 0, 0}};
      mesh->idxs[ix] = ix;
      ix++;
    }
    mesh->uploadData();

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    // glEnable(GL_CULL_FACE);
    // glEnable(GL_DEPTH_TEST); // render all star fragments -> no depth test
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  }

  void onRender([[maybe_unused]] float time, [[maybe_unused]] float deltaTime) final
  {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static float speed = 0.1f;
    static int numIter = 1;
    float period = deltaTime * speed;
    solver->update(period, numIter);

    for (size_t ix = 0; const auto &obj : objects)
      mesh->verts[ix++].position = {obj.pos.x, obj.pos.y, 0};

    mesh->uploadData();

    ImGui::Begin("Verlet Simulation");
    ImGui::Text("Frame dur: %.4f, FPS: %.1f", deltaTime, 1.0f / deltaTime);
    ImGui::InputFloat("Speed", &speed, 0.001f, 0, "%.4f", ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SliderInt("NumIter", &numIter, 1, 5);
    ImGui::Text("Potential: %+3.2e, Kinetic: %+3.2e, Total: %+3.2e", solver->potential, solver->kinetic, solver->potential + solver->kinetic);
    static float areaSize = 10.0f;
    ImGui::SliderFloat("Area Size", &areaSize, 0.1f, 100.f, "%3.1f");

    ImGui::Separator();
    static bool showImPlotDemo = false;
    ImGui::Checkbox("ImPlot Demo", &showImPlotDemo);
    if (showImPlotDemo)
      ImPlot::ShowDemoWindow();

    static EnergiesPlot eplt{5 * 60}; // approx N sec in 60 FPS
    eplt.addEnergyPoints(time, solver->potential, solver->kinetic, solver->potential + solver->kinetic);
    eplt.plot({-1, 600});
    ImGui::End();

    float rts[2] = {static_cast<float>(width), static_cast<float>(height)};
    const glm::mat4 projOrtho = glm::ortho(-areaSize, areaSize, -areaSize, areaSize, -1.f, 1.f);

    pointShader->bind();
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