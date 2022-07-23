/**
 * "How to do units and scaling right (Or at least how to NOT do them wrong)"
 * https://gandalfcode.github.io/gandalf-school/Units.pdf
 *
 * r' = r / R0, m' = m / M0, t' = t / T0
 * v' = v / V0 = v T0 / R0, V0 = R0 / T0
 * a' = a / A0 = a T0^2 / R0, A0 = R0 / T0^2
 *
 * a = G m / r^2
 * a' R0 / T0^2 = G m' M0 / r'^2 R0^2
 * a' = (G M0 T0^2 / R0^3) m' / r'^2
 * G' = G M0 T0^2 / R0^3
 * G = 6.674 10^−11 m^3 / kg s^2
 * choose T0 one day, M0 earth's mass, R0 distance between earth and sun
 */
#include "Verlet.h"
#include "plots.h"

#include <GSAssets.h>
#include <App.h>
#include <Camera.h>
#include <CameraController.h>
#include <Mesh.h>
#include <Shader.h>

#include <glad/gl.h>
#include <glm/vec2.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include <array>
#include <cmath>
#include <functional>
#include <unordered_map>
#include <memory>
#include <random>
#include <vector>

// https://home.ifa.hawaii.edu/users/barnes/research/smoothing/soft.pdf
float softening = 0.000001f;

namespace constants
{
  // T0: 1 day in seconds
  const float T0 = 24 * 60 * 60;

  // M0: Earth's mass in kg
  // https://en.wikipedia.org/wiki/Earth_mass
  const float M0 = 5.972e24f;

  // R0: // distance from earth to sun in meters = 1 AU = 150 Mkm (varies 3% throughout the year)
  // https://en.wikipedia.org/wiki/Astronomical_unit
  const float R0 = 150e9f;

  // G: gravitational constant G in m^3 / kg s^2
  // https://en.wikipedia.org/wiki/Gravitational_constant
  const float GG = 6.674e-11f;

  // G0: unitless gravitational constant = 8.81576e-10
  const float G0 = GG * M0 * std::pow(T0, 2.f) / std::pow(R0, 3.f);

  const float R_AU = 1.0f;

  const float M_Earth = 1.0f;

  // Sun's mass is 333030 M0
  const float M_Sun = 333030.0f;

  // Earth's speed = 29.78 km/s
  // https://en.wikipedia.org/wiki/Earth%27s_orbit
  const float V_Earth_Sun = 2.978e4f * T0 / R0;

  // Moon's speed around Earth: 1.022km/s
  // https://en.wikipedia.org/wiki/Orbit_of_the_Moon
  const float V_Moon_Earth = 1022 * T0 / R0;
  const float R_Moon_Earth = 0.00257f;
  const float M_Moon = 1.0f / 82;
} // namespace constants

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
  std::unique_ptr<ws::Shader> lineShader;
  std::unique_ptr<ws::Mesh> mesh;
  std::unique_ptr<ws::Mesh> debugMesh;

  std::unique_ptr<ws::Camera2D> camera;
  std::unique_ptr<ws::Camera2DController> camController;

  InterForce gravitationalForce = [](const VerletObject &obj1, const VerletObject &obj2)
  {
    glm::vec2 r = obj1.pos - obj2.pos;
    const float r2 = glm::dot(r, r);
    return constants::G0 * obj1.mass * obj2.mass * r / glm::pow(r2 + softening, 1.5f);
  };

  InterPotential gravitationalPotential = [](const VerletObject &obj1, const VerletObject &obj2)
  {
    glm::vec2 r = obj1.pos - obj2.pos;
    const float r2 = glm::dot(r, r);
    return -constants::G0 * obj1.mass * obj2.mass / glm::pow(r2 + softening, 0.5f);
  };

  InterPotential gravitationalPotentialOriginal = [](const VerletObject &obj1, const VerletObject &obj2)
  {
    glm::vec2 r = obj1.pos - obj2.pos;
    const float r2 = glm::dot(r, r);
    return -constants::G0 * obj1.mass * obj2.mass / glm::pow(r2, 0.5f);
  };

  std::mt19937 rndGen;
  std::uniform_real_distribution<float> rndDist;

  MyApp() : App({.name = "MyApp", .width = 800u, .height = 800u, .shouldDebugOpenGL = true}) {}

  void setupSolarSystemFilledWithPlanets(int numObjects, float speedFactor)
  {
    objects.clear();
    objects.emplace_back(VerletObject{{0, 0}, {0, 0}, constants::M_Sun, 0.05f, {}});
    for (int n = 0; n < numObjects; n++)
    {
      const float r = constants::R_AU * rndDist(rndGen);
      glm::vec2 p{};
      {
        const float theta = 2.0f * 3.14159265f * rndDist(rndGen);
        const float x = r * std::cos(theta);
        const float y = r * std::sin(theta);
        p = {x, y};
      }
      glm::vec2 v{};
      {
        const float speed = constants::V_Earth_Sun / std::sqrt(r);
        v = glm::normalize(glm::vec2{-p.y, p.x}) * speed * speedFactor;
      }

      objects.emplace_back(VerletObject{p, v, 1.5f, 0.01f});
    }

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
  }

  void setupSunEarthMoon()
  {
    objects.clear();
    // Add sun
    objects.emplace_back(VerletObject{{0, 0}, {0, 0}, constants::M_Sun, 0.05f, {}});
    // Add earth
    objects.emplace_back(VerletObject{{constants::R_AU, 0}, {0, constants::V_Earth_Sun}, constants::M_Earth, 0.002f, {}});
    // Add moon
    objects.emplace_back(VerletObject{{constants::R_AU - constants::R_Moon_Earth, 0}, {0, constants::V_Earth_Sun - constants::V_Moon_Earth}, constants::M_Moon, 0.0002f, {}});

    // Earth+Moon only
    // objects.emplace_back(VerletObject{{0, 0}, {0, 0}, constants::M_Earth, 0.002f, {}});
    // objects.emplace_back(VerletObject{{0.00257, 0}, {0, constants::V_Moon}, 1.0f / 82, 0.0002f, {}});

    mesh = std::make_unique<ws::Mesh>(objects.size());
    for (uint32_t ix = 0; const auto &obj : objects)
    {
      mesh->verts[ix] = ws::DefaultVertex{{obj.pos.x, obj.pos.y, 0}, {}, {}, {1, 1, 1, 1}, {obj.radius, 0, 0, 0}};
      mesh->idxs[ix] = ix;
      ix++;
    }
    mesh->uploadData();
  }

  void onInit() final
  {
    pointShader = std::make_unique<ws::Shader>();
    pointShader->load(
        GS_ASSETS_FOLDER / "shaders/graverlet/main.vert",
        GS_ASSETS_FOLDER / "shaders/graverlet/point.frag");

    lineShader = std::make_unique<ws::Shader>(GS_ASSETS_FOLDER / "shaders/graverlet/main.vert",
                                              GS_ASSETS_FOLDER / "shaders/graverlet/line.frag");

    setupSunEarthMoon();
    solver = std::make_unique<Solver>(objects, gravitationalForce, gravitationalPotential);

    camera = std::make_unique<ws::Camera2D>(2.5f, 2.5f);
    camController = std::make_unique<ws::Camera2DController>(*camera);

    debugMesh = std::make_unique<ws::Mesh>(1024);

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

    static float cellSize = 0.1f;

    static float speed = 30.0f;
    static int numIter = 2;
    float period = deltaTime * speed;
    static bool useApproximation = false;
    SpatialAccelarator *sa = nullptr;
    if (!useApproximation)
    {
      solver->update(period, numIter);
      sa = new SpatialAccelarator{objects, cellSize};
    }
    else
      solver->updateOptimized(period, numIter, cellSize, sa);

    for (size_t ix = 0; const auto &obj : objects)
      mesh->verts[ix++].position = {obj.pos.x, obj.pos.y, 0};

    mesh->uploadData();

    const float widthF = static_cast<float>(width);
    const float heightF = static_cast<float>(height);
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(getWinPosX()), static_cast<float>(getWinPosY())), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(widthF, heightF), ImGuiCond_Always);
    ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse);
    camController->update(widthF, heightF);
    ImGui::End();

    ImGui::Begin("Verlet Simulation");
    ImGui::Text("Frame dur: %.4f, FPS: %.1f", deltaTime, 1.0f / deltaTime);

    ImGui::Separator();
    ImGui::SliderFloat("cellSize", &cellSize, 0.001f, 0.5f, "%.4f");
    // if (ImGui::Button("Objs in SA"))
    //   sa.debugPrint();
    ImGui::SameLine();
    static bool showAccGrid = true;
    ImGui::Checkbox("Show Acc Grid", &showAccGrid);
    static int selObjIx = 0;
    ImGui::InputInt("Selected Object", &selObjIx, 1, 10, ImGuiInputTextFlags_EnterReturnsTrue);
    // if (ImGui::Button("List Neighbors"))
    // {
    //   printf("Listing neighbors...\n");
    //   for (auto &obj : sa.neighborsOf(objects[selObjIx]))
    //   {
    //     printf("(%g, %g)\n", obj.pos.x, obj.pos.y);
    //   }
    // }
    ImGui::Checkbox("Approximate", &useApproximation);

    ImGui::Separator();
    static int numObjects = 200;
    static float speedFactor = 1.0f;
    ImGui::InputInt("Num Objects", &numObjects, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SliderFloat("Speed Factor", &speedFactor, 0.00001f, 10.0f);
    if (ImGui::Button("Sun with N planets"))
      setupSolarSystemFilledWithPlanets(numObjects, speedFactor);
    ImGui::SameLine();
    if (ImGui::Button("Sun, Earth, Moon"))
      setupSunEarthMoon();

    ImGui::Separator();

    ImGui::InputFloat("Softening", &softening, 0.001f, 0.1f, "%.8f", ImGuiInputTextFlags_EnterReturnsTrue);
    plotOriginalAndSoftenedGravitationalForces(gravitationalPotentialOriginal, gravitationalPotential, 2.0f, -1e-7f);

    ImGui::Separator();
    ImGui::InputFloat("Speed (days/sec)", &speed, 0.001f, 0, "%.4f", ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SliderInt("NumIter", &numIter, 1, 16);
    ImGui::Text("cam pos: (%g, %G), size: (%g, %G)", camera->position.x, camera->position.y, camera->width, camera->height);
    static int objIx = -1;
    ImGui::InputInt("Camera Follows Object", &objIx, 1, 10, ImGuiInputTextFlags_EnterReturnsTrue);
    if (objIx > -1)
      camera->position = objects[objIx].pos;

    ImGui::Separator();
    static bool showImPlotDemo = false;
    ImGui::Checkbox("ImPlot Demo", &showImPlotDemo);
    if (showImPlotDemo)
      ImPlot::ShowDemoWindow();
    ImGui::SameLine();
    static bool showImGuiDemo = false;
    ImGui::Checkbox("ImGui Demo", &showImGuiDemo);
    if (showImGuiDemo)
      ImGui::ShowDemoWindow();

    ImGui::Text("Potential: %+3.2e, Kinetic: %+3.2e, Total: %+3.2e", solver->potential, solver->kinetic, solver->potential + solver->kinetic);
    static EnergiesPlot eplt{5 * 60}; // approx N sec in 60 FPS
    eplt.addEnergyPoints(time, solver->potential, solver->kinetic, solver->potential + solver->kinetic);
    eplt.plot({-1, 600});
    ImGui::End();

    float rts[2] = {widthF, heightF};

    debugMesh->verts.clear();
    debugMesh->idxs.clear();
    uint32_t saGridIdx = 0;
    const std::array<uint32_t, 8> relativeIdxs = {0, 1, 1, 2, 2, 3, 3, 0};
    const std::array<glm::vec2, 4> relativePoses = {glm::vec2{0, 0}, {1, 0}, {1, 1}, {0, 1}};

    if (sa != nullptr)
    {
      for (auto &[key, vec] : sa->cache)
      {
        const float x = static_cast<float>(key.first) * cellSize;
        const float y = static_cast<float>(key.second) * cellSize;
        const float z = -0.1f;
        for (const auto &rp : relativePoses)
          debugMesh->verts.emplace_back(glm::vec3{x + rp.x * cellSize, y + rp.y * cellSize, z});

        for (auto relIx : relativeIdxs)
          debugMesh->idxs.push_back(saGridIdx + relIx);
        saGridIdx += static_cast<uint32_t>(relativePoses.size());
      }
      delete sa;
      debugMesh->uploadData();
    }

    pointShader->bind();
    pointShader->setVector2fv("RenderTargetSize", rts);
    pointShader->setMatrix4fv("ProjectionFromView", glm::value_ptr(camera->getProjectionFromView()));
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_POINTS, static_cast<GLsizei>(mesh->idxs.size()), GL_UNSIGNED_INT, 0);

    if (showAccGrid)
    {
      lineShader->bind();
      lineShader->setVector2fv("RenderTargetSize", rts);
      lineShader->setMatrix4fv("ProjectionFromView", glm::value_ptr(camera->getProjectionFromView()));
      glBindVertexArray(debugMesh->vao);
      glDrawElements(GL_LINES, static_cast<GLsizei>(debugMesh->idxs.size()), GL_UNSIGNED_INT, 0);
    }
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