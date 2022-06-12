#include "App.h"
#include "Shader.h"
#include "Mesh.h"
#include "OMesh.h"
#include "Camera.h"

#include <glad/gl.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <string>
#include <memory>

#include <iostream>

class MyApp : public ws::App
{
public:
  // Because these don't have default constructors, can't make them members in class scope
  std::unique_ptr<ws::Shader> mainShader;
  ws::OMesh *oMesh;
  std::unique_ptr<ws::Mesh> mesh;
  std::unique_ptr<ws::Camera> camera;

  Specs getSpecs() final
  {
    return {.name = "MyApp", .width = 800u, .height = 600u, .shouldDebugOpenGL = true};
  }

  void onInit() final
  {
    const char *vertexShaderSource = R"(
#version 460 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;

uniform mat4 WorldFromObject;
uniform mat4 ViewFromWorld;
uniform mat4 ProjectionFromView;

void main()
{
  gl_Position = ProjectionFromView * ViewFromWorld * WorldFromObject * vec4(vPos, 1.0);
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

    camera = std::make_unique<ws::Camera>(static_cast<float>(specs.width), static_cast<float>(specs.height));

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  void onRender(float time, float deltaTime) final
  {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    static bool showDemo = false;
    static bool shouldOrbitCamera = false;
    ImGui::Begin("Main");
    ImGui::Checkbox("Orbit Camera", &shouldOrbitCamera);
    ImGui::Separator();
    ImGui::Checkbox("Demo", &showDemo);
    ImGui::End();
    if (showDemo)
      ImGui::ShowDemoWindow();

    if (shouldOrbitCamera)
      camera->position = glm::rotate(camera->position, 1.0f * deltaTime, camera->UP);

    glUseProgram(mainShader->id);
    mainShader->setMatrix4fv("ViewFromWorld", glm::value_ptr(camera->getViewFromWorld()));
    mainShader->setMatrix4fv("ProjectionFromView", glm::value_ptr(camera->getProjectionFromView()));

    glBindVertexArray(mesh->vao);
    mainShader->setMatrix4fv("WorldFromObject", glm::value_ptr(glm::mat4(1.f)));
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
