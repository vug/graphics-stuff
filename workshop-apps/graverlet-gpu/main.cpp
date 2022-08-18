#include <App.h>
#include <Framebuffer.h>
#include <GSAssets.h>
#include <Mesh.h>
#include <Shader.h>
#include <Texture.h>

#include <glad/gl.h>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <implot.h>

#include <array>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

class GraverletGPU : public ws::App
{
public:
  // because we are storing the pos,vel,acc in a texture, max number of particles is determined by max texture size calculated below
  static constexpr uint32_t MAX_PARTICLES = 32'768u;
  std::unordered_map<std::string, std::unique_ptr<ws::Shader>> shaders;
  std::unordered_map<std::string, std::unique_ptr<ws::Texture>> textures;

  std::unique_ptr<ws::Mesh> mesh;
  std::unique_ptr<ws::Mesh> meshQuad;
  std::unique_ptr<ws::Framebuffer> framebuffer;
  std::unique_ptr<ws::Framebuffer> framebuffer2;
  uint32_t numParticles = 100;
  float softening = 0.01f;

  std::mt19937 rndGen;
  std::uniform_real_distribution<float> rndDist;

  GraverletGPU() : App({.name = "MyApp", .width = 800u, .height = 600u, .shouldDebugOpenGL = true, .shouldBreakAtOpenGLDebugCallback = true}) {}

  void onInit() final
  {
    {
      int maxTextureSize;
      glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
      printf("Max Texture Size: %d\n", maxTextureSize);
    }

    shaders["point"] = std::make_unique<ws::Shader>(GS_ASSETS_FOLDER / "shaders/graverlet/main.vert",
                                                    GS_ASSETS_FOLDER / "shaders/graverlet/point.frag");
    shaders["quad"] = std::make_unique<ws::Shader>(GS_ASSETS_FOLDER / "shaders/postprocess/main.vert",
                                                   GS_ASSETS_FOLDER / "shaders/postprocess/main.frag");
    shaders["compute"] = std::make_unique<ws::Shader>(GS_ASSETS_FOLDER / "shaders/graverlet/graverlet.comp");

    // cannot be allocated on stack
    auto initialState = std::make_unique<std::array<std::array<glm::vec4, MAX_PARTICLES>, 3>>();
    for (uint32_t i = 0; i < numParticles; ++i)
    {
      const float px = rndDist(rndGen) - 0.5f;
      const float py = rndDist(rndGen) - 0.5f;
      const float pz = 0.f;
      const glm::vec4 pos = {px, py, pz, 0};
      const float vx = 0.0f * (rndDist(rndGen) - 0.5f);
      const float vy = 0.0f * (rndDist(rndGen) - 0.5f);
      const float vz = 0.f;
      const glm::vec4 vel = {vx, vy, vz, 0};
      (*initialState)[0][i] = pos;
      (*initialState)[1][i] = vel;
      (*initialState)[2][i] = {0, 0, 0, 0};
    }
    // auto gravitationalForce = [](const glm::vec3 &posA, const glm::vec3 &posB, float softening)
    // {
    //   glm::vec3 r = posA - posB;
    //   const float r2 = glm::dot(r, r);
    //   return -r / glm::pow(r2 + softening, 1.5f);
    // };
    // for (uint32_t i = 0; i < numParticles; ++i)
    // {
    //   glm::vec3 accA{};
    //   for (uint32_t j = 0; j < numParticles; ++j)
    //   {
    //     const glm::vec4 &pA = (*initialState)[0][i];
    //     const glm::vec4 &pB = (*initialState)[0][j];
    //     const glm::vec3 posA = {pA.x, pA.y, pA.z};
    //     const glm::vec3 posB = {pB.x, pB.y, pB.z};
    //     accA += gravitationalForce(posA, posB, softening);
    //   }
    //   (*initialState)[2][i] = {accA.x, accA.y, accA.z, 0};
    // }
    textures["state"] = std::make_unique<ws::Texture>(ws::Texture::Specs{MAX_PARTICLES, 3, ws::Texture::Format::RGBA32f, ws::Texture::Filter::Nearest, ws::Texture::Wrap::ClampToBorder, initialState->data()});
    textures["stateNext"] = std::make_unique<ws::Texture>(ws::Texture::Specs{MAX_PARTICLES, 3, ws::Texture::Format::RGBA32f, ws::Texture::Filter::Nearest, ws::Texture::Wrap::ClampToBorder, nullptr});

    mesh.reset(new ws::Mesh(numParticles, ws::Mesh::Type::Points));

    meshQuad.reset(new ws::Mesh(ws::Mesh::makeQuad()));

    framebuffer = std::make_unique<ws::Framebuffer>(width, height);

    glEnable(GL_PROGRAM_POINT_SIZE);
  }

  void onRender([[maybe_unused]] float time, [[maybe_unused]] float deltaTime) final
  {
    const float widthF = static_cast<float>(width);
    const float heightF = static_cast<float>(height);
    const float renderTargetSize[2] = {widthF, heightF};

    ImGui::Begin("Boilerplate");
    static float zoom = 10.0f;
    ImGui::InputFloat("zoom", &zoom, 0, 0, "%3.f", ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::Separator();
    static bool showImGuiDemo = false;
    static bool showImPlotDemo = false;
    ImGui::Checkbox("Show ImGui Demo", &showImGuiDemo);
    ImGui::Checkbox("Show ImPlot Demo", &showImPlotDemo);
    if (showImGuiDemo)
      ImGui::ShowDemoWindow();
    if (showImPlotDemo)
      ImPlot::ShowDemoWindow();
    ImGui::Separator();
    if (ImGui::Button("Reload"))
      for (auto &[name, shader] : shaders)
        shader->reload();
    ImGui::End();

    ws::Shader &compute = *shaders["compute"];
    compute.bind();
    glUniform1f(glGetUniformLocation(compute.getId(), "u_dt"), deltaTime);
    glUniform1i(glGetUniformLocation(compute.getId(), "numParticles"), numParticles);
    glUniform1f(glGetUniformLocation(compute.getId(), "softening"), softening);
    textures["state"]->bindImageTexture(0, ws::Texture::Access::Read);
    textures["stateNext"]->bindImageTexture(1, ws::Texture::Access::Write);
    ws::Shader::dispatchCompute(numParticles, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    std::unique_ptr<glm::vec4[]> computeData = std::make_unique<glm::vec4[]>(numParticles * 3);
    glGetTextureSubImage(textures["stateNext"]->getId(), 0, 0, 0, 0, numParticles, 3, 1, GL_RGBA, GL_FLOAT, numParticles * 3 * sizeof(glm::vec4), computeData.get());
    mesh->verts.clear();
    mesh->idxs.clear();
    for (uint32_t n = 0; n < numParticles; ++n)
    {
      const uint32_t ixPos = n;
      // const uint32_t ixVel = ixPos + numParticles;
      // const uint32_t ixAcc = ixVel + numParticles;
      // printf("[%u] (%.2e, %.2e, %.2e), (%.2e, %.2e, %.2e), (%.2e, %.2e, %.2e)\n", n,
      //        computeData[ixPos].x, computeData[ixPos].y, computeData[ixPos].z,
      //        computeData[ixVel].x, computeData[ixVel].y, computeData[ixVel].z,
      //        computeData[ixAcc].x, computeData[ixAcc].y, computeData[ixAcc].z);
      mesh->verts.push_back({{computeData[ixPos].x, computeData[ixPos].y, computeData[ixPos].z}});
      mesh->verts.back().custom1.x = 0.01f;
      mesh->idxs.push_back(n);
    }
    // printf("\n");
    mesh->uploadData();

    glCopyImageSubData(textures["stateNext"]->getId(), GL_TEXTURE_2D, 0, 0, 0, 0,
                       textures["state"]->getId(), GL_TEXTURE_2D, 0, 0, 0, 0,
                       numParticles, 3, 1);

    // {
    // framebuffer->bind();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ws::Shader &shader = *shaders["point"];
    shader.bind();
    shader.setVector2fv("RenderTargetSize", renderTargetSize);
    auto proj = glm::ortho(-zoom, zoom, -zoom, zoom, -1.f, 1.f);
    shader.setMatrix4fv("ProjectionFromView", glm::value_ptr(proj));
    mesh->bind();
    mesh->draw();
    //   framebuffer->unbind();
    // }

    // {
    //   glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    //   glClear(GL_COLOR_BUFFER_BIT);
    //   ws::Shader &shader = *shaders["quad"];
    //   shader.bind();
    //   shader.setVector2fv("RenderTargetSize", renderTargetSize);
    //   meshQuad->bind();
    //   glDisable(GL_DEPTH_TEST);
    //   glBindTexture(GL_TEXTURE_2D, framebuffer->getColorAttachment().getId());
    //   meshQuad->draw();
    // }
  }

  void onDeinit() final
  {
  }
};

int main()
{
  GraverletGPU app;
  app.run();
}