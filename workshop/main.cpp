/**
 * bring OpenGL functions via Glad. Set clear color.
 */
#include "App.h"

#include <glad/gl.h>

#include <string>

// TODO: remove
#include <iostream>

class MyApp : public ws::App
{
  Specs getSpecs() final
  {
    return {"MyApp", 800, 600};
  }
  void onInit() final
  {
    std::cout << "Initializing MyApp\n";
  }
  void onRender() final
  {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
  }
  void onDeinit() final
  {
    std::cout << "Deinitializing MyApp\n";
  }
};

int main()
{
  MyApp app;
  app.run();
  return 0;
}
