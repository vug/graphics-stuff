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
  uint32_t numParticles = 2;

  GraverletGPU() : App({.name = "MyApp", .width = 800u, .height = 600u, .shouldDebugOpenGL = true, .shouldBreakAtOpenGLDebugCallback = true}) {}

  void onInit() final
  {
    {
      int maxTextureSize;
      glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
      printf("Max Texture Size: %d", maxTextureSize);
    }

    shaders["main"] = std::make_unique<ws::Shader>(GS_ASSETS_FOLDER / "shaders/graverlet/main.vert",
                                                   GS_ASSETS_FOLDER / "shaders/graverlet/line.frag");
    shaders["quad"] = std::make_unique<ws::Shader>(GS_ASSETS_FOLDER / "shaders/postprocess/main.vert",
                                                   GS_ASSETS_FOLDER / "shaders/postprocess/main.frag");

    // cannot be allocated on stack
    auto initialState = std::make_unique<std::array<std::array<glm::vec4, MAX_PARTICLES>, 3>>();
    (*initialState)[0][0] = {-1, 0, 0, 0};
    (*initialState)[0][1] = {+1, 0, 0, 0};
    textures["state"] = std::make_unique<ws::Texture>(ws::Texture::Specs{MAX_PARTICLES, 3, ws::Texture::Format::RGBA32f, ws::Texture::Filter::Nearest, ws::Texture::Wrap::ClampToBorder, initialState->data()});
    textures["stateNext"] = std::make_unique<ws::Texture>(ws::Texture::Specs{MAX_PARTICLES, 3, ws::Texture::Format::RGBA32f, ws::Texture::Filter::Nearest, ws::Texture::Wrap::ClampToBorder, nullptr});

    mesh.reset(new ws::Mesh(ws::Mesh::makeQuadLines()));

    meshQuad.reset(new ws::Mesh(ws::Mesh::makeQuad()));

    framebuffer = std::make_unique<ws::Framebuffer>(width, height);
  }

  void onRender([[maybe_unused]] float time, [[maybe_unused]] float deltaTime) final
  {
    const float widthF = static_cast<float>(width);
    const float heightF = static_cast<float>(height);
    const float renderTargetSize[2] = {widthF, heightF};

    ImGui::Begin("Boilerplate");
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

    // ws::Texture::activateTexture(0);
    textures["state"]->bind();
    textures["state"]->bindImageTexture(0, ws::Texture::Access::Read);

    // std::array<std::array<glm::vec4, MAX_PARTICLES>, 3> computeData;
    // glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, computeData.data());

    // std::array<std::array<glm::vec4, MAX_PARTICLES>, 3> computeData;
    // int size = computeData.size() * computeData[0].size() * sizeof(glm::vec4);
    // glGetTextureSubImage(textures["state"]->getId(), 0, 0, 0, 0, MAX_PARTICLES, 3, 1, GL_RGBA, GL_FLOAT, size, computeData.data());

    // std::array<std::array<glm::vec4, 2>, 3> computeData;
    // int size = computeData.size() * computeData[0].size() * sizeof(glm::vec4);
    // glGetTextureSubImage(textures["state"]->getId(), 0, 0, 0, 0, numParticles, 3, 1, GL_RGBA, GL_FLOAT, size, computeData.data());

    // for (auto &v : computeData[0])
    //   printf("(%.1f, %.1f, %.1f) ", v.x, v.y, v.z);
    // printf("\n");

    // below won't work because vector of vector (each resized separately) is not contigous memory
    std::unique_ptr<glm::vec4[]> computeData = std::make_unique<glm::vec4[]>(numParticles * 3);
    glGetTextureSubImage(textures["state"]->getId(), 0, 0, 0, 0, numParticles, 3, 1, GL_RGBA, GL_FLOAT, numParticles * 3 * sizeof(glm::vec4), computeData.get());
    for (int i = 0; i < numParticles; ++i)
      printf("(%.1f, %.1f, %.1f) ", computeData[i].x, computeData[i].y, computeData[i].z);
    printf("\n");

    textures["state"]->unbind();

    {
      framebuffer->bind();
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      ws::Shader &shader = *shaders["main"];
      shader.bind();
      shader.setVector2fv("RenderTargetSize", renderTargetSize);
      auto proj = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -1.f, 1.f);
      shader.setMatrix4fv("ProjectionFromView", glm::value_ptr(proj));
      mesh->bind();
      mesh->draw();
      framebuffer->unbind();
    }

    {
      glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      ws::Shader &shader = *shaders["quad"];
      shader.bind();
      shader.setVector2fv("RenderTargetSize", renderTargetSize);
      meshQuad->bind();
      glDisable(GL_DEPTH_TEST);
      glBindTexture(GL_TEXTURE_2D, framebuffer->getColorAttachment().getId());
      meshQuad->draw();
    }
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