#include <App.h>

#include <imgui.h>
#include <implot.h>

class Boilerplate : public ws::App
{
public:
  Boilerplate() : App({.name = "MyApp", .width = 800u, .height = 600u, .shouldDebugOpenGL = true}) {}

  void onInit() final
  {
  }

  void onRender([[maybe_unused]] float time, [[maybe_unused]] float deltaTime) final
  {
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