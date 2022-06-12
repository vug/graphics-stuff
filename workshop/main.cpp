/**
 * bring OpenGL functions via Glad. Set clear color.
 */
#include "App.h"
#include "Shader.h"
#include "Mesh.h"

#include <glad/gl.h>
#include <imgui.h>

#include <string>
#include <memory>

#include <iostream>

class MyApp : public ws::App
{
public:
  // Because these don't have default constructors, can't make them members in class scope
  std::unique_ptr<ws::Shader> mainShader;
  std::unique_ptr<ws::Mesh> mesh;
  ws::OMesh *oMesh;

  Specs getSpecs() final
  {
    return {.name = "MyApp", .width = 800u, .height = 600u, .shouldDebugOpenGL = true};
  }

  void onInit() final
  {
    const char *vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
void main()
{
    gl_Position = vec4(aPos, 1.0);
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
    mainShader = std::make_unique<ws::Shader>(vertexShaderSource, fragmentShaderSource);

    oMesh = ws::makeIcosphereOMesh(1);
    mesh.reset(ws::makeMeshFromOMesh(*oMesh));

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  void onRender() final
  {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    static bool showDemo = false;
    ImGui::Begin("Main");
    ImGui::Checkbox("Demo", &showDemo);
    ImGui::End();
    if (showDemo)
      ImGui::ShowDemoWindow();

    glUseProgram(mainShader->id);
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->idxs.size()), GL_UNSIGNED_INT, 0);
  }

  void onDeinit() final
  {
    delete oMesh;
  }
};

int main()
{
  MyApp app;
  app.run();
  return 0;
}
