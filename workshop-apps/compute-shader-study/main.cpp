#include <App.h>
#include <Framebuffer.h>
#include <GSAssets.h>
#include <Mesh.h>
#include <Shader.h>
#include <Texture.h>

#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <implot.h>

#include <memory>
#include <string>
#include <unordered_map>

using namespace std::literals::string_literals;

class FirstComputeShader : public ws::App
{
public:
  std::unordered_map<std::string, std::unique_ptr<ws::Shader>> shaders;
  std::unordered_map<std::string, std::unique_ptr<ws::Texture>> textures;
  std::unique_ptr<ws::Mesh> mesh;
  std::unique_ptr<ws::Mesh> meshQuad;
  std::unique_ptr<ws::Framebuffer> framebuffer;
  std::unique_ptr<ws::Framebuffer> framebuffer2;
  const glm::uvec2 compTexSize = {10, 1};

  FirstComputeShader() : App({.name = "ComputeShaderStudy", .width = 800u, .height = 600u, .shouldDebugOpenGL = true}) {}

  void onInit() final
  {
    shaders["main"] = std::make_unique<ws::Shader>(GS_ASSETS_FOLDER / "shaders/graverlet/main.vert",
                                                   GS_ASSETS_FOLDER / "shaders/graverlet/line.frag");
    shaders["quad"] = std::make_unique<ws::Shader>(GS_ASSETS_FOLDER / "shaders/postprocess/main.vert",
                                                   GS_ASSETS_FOLDER / "shaders/postprocess/main.frag");
    shaders["compute"] = std::make_unique<ws::Shader>(GS_ASSETS_FOLDER / "shaders/compute-shader-study/first.comp");

    float compTextInitialValues[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    textures["computeInOut"] = std::make_unique<ws::Texture>(ws::Texture::Specs{compTexSize.x, compTexSize.y, ws::Texture::Format::R32f, ws::Texture::Filter::Nearest, ws::Texture::Wrap::ClampToBorder, compTextInitialValues});

    mesh.reset(new ws::Mesh(ws::Mesh::makeQuadLines()));

    meshQuad.reset(new ws::Mesh(ws::Mesh::makeQuad()));

    framebuffer = std::make_unique<ws::Framebuffer>(width, height);
  }

  void onRender([[maybe_unused]] float time, [[maybe_unused]] float deltaTime) final
  {
    const float widthF = static_cast<float>(width);
    const float heightF = static_cast<float>(height);
    const float renderTargetSize[2] = {widthF, heightF};

    ImGui::Begin((specs.name + " Settings"s).c_str());
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

    shaders["compute"]->bind();
    ws::Texture::activateTexture(0);
    textures["computeInOut"]->bind();
    textures["computeInOut"]->bindImageTexture(0, ws::Texture::Access::ReadAndWrite);
    // glUniform1i(DiffuseTextureID, 0); or glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0); would have been needed to use sampler2d in a fragment shader?
    ws::Shader::dispatchCompute(compTexSize.x, compTexSize.y, 1);
    // glDispatchCompute(compTexSize.x, compTexSize.y, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    unsigned int collection_size = compTexSize.x * compTexSize.y;
    std::vector<float> compute_data(collection_size);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, compute_data.data());
    for (const float x : compute_data)
      printf("%.1f ", x);
    printf("\n");
    textures["computeInOut"]->unbind();

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
  FirstComputeShader app;
  app.run();
}