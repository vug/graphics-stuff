#include <App.h>

class Boilerplate : public ws::App
{
public:
  Boilerplate() : App({.name = "MyApp", .width = 800u, .height = 600u, .shouldDebugOpenGL = true}) {}

  void onInit() final
  {
  }

  void onRender([[maybe_unused]] float time, [[maybe_unused]] float deltaTime) final
  {
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