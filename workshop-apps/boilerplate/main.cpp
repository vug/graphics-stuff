#include <App.h>
#include <GSAssets.h>
#include <Mesh.h>
#include <Shader.h>

#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <implot.h>

#include <memory>

class Boilerplate : public ws::App
{
public:
  std::unique_ptr<ws::Shader> shader;
  std::unique_ptr<ws::Mesh> mesh;

  Boilerplate() : App({.name = "MyApp", .width = 800u, .height = 600u, .shouldDebugOpenGL = true}) {}

  void onInit() final
  {
    shader = std::make_unique<ws::Shader>(GS_ASSETS_FOLDER / "shaders/graverlet/main.vert",
                                          GS_ASSETS_FOLDER / "shaders/graverlet/line.frag");
    mesh.reset(new ws::Mesh(ws::Mesh::makeQuadLines()));
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
    ImGui::End();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->bind();
    shader->setVector2fv("RenderTargetSize", renderTargetSize);
    auto proj = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -1.f, 1.f);
    shader->setMatrix4fv("ProjectionFromView", glm::value_ptr(proj));
    mesh->bind();
    glDrawElements(GL_LINES, static_cast<GLsizei>(mesh->idxs.size()), GL_UNSIGNED_INT, 0);
  }

  void onDeinit() final
  {
  }
};

int main()
{
  Boilerplate app;
  app.run();
}