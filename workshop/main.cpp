/**
 * bring OpenGL functions via Glad. Set clear color.
 */
#include "App.h"
#include "Shader.h"

#include <glad/gl.h>

#include <string>

// TODO: remove
#include <iostream>

class MyApp : public ws::App
{
  const char *vertexShaderSource = R"(
  #version 460 core
  layout (location = 0) in vec3 aPos;
  void main()
  {
      gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
  }
  )";
  const char *fragmentShaderSource = R"(
  #version 460 core
  out vec4 FragColor;
  void main()
  {
      FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
  }
  )";

  Specs getSpecs() final
  {
    return {"MyApp", 800, 600};
  }
  void onInit() final
  {
    ws::Shader shader{vertexShaderSource, fragmentShaderSource};
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
