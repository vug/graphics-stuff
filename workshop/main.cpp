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

    // Get positions of a vertex and it's two neighbors
    static int vertexNo = 0;
    static int neighbor1Ix = 0;
    static int neighbor2Ix = 1;
    static int numNeighbors = ws::getOMeshNumNeighbors(*oMesh, vertexNo);
    static glm::vec3 vPos = ws::getOMeshVertexPosition(*oMesh, vertexNo);
    static std::vector<int> neighborIndices = ws::getOMeshVertexNeighborIndices(*oMesh, vertexNo);
    static glm::vec3 n1Pos = ws::getOMeshVertexPosition(*oMesh, neighborIndices[neighbor1Ix]);
    static glm::vec3 n2Pos = ws::getOMeshVertexPosition(*oMesh, neighborIndices[neighbor2Ix]);

    const int numVerts = ws::getOMeshNumVertices(*oMesh);
    bool hasVertexChanged = ImGui::DragInt("vertex no", &vertexNo, 1, 0, numVerts - 1, "%d", ImGuiSliderFlags_None);
    if (hasVertexChanged)
    {
      vPos = ws::getOMeshVertexPosition(*oMesh, vertexNo);
      neighborIndices = ws::getOMeshVertexNeighborIndices(*oMesh, vertexNo);
      numNeighbors = ws::getOMeshNumNeighbors(*oMesh, vertexNo);
      if (neighbor1Ix >= numNeighbors)
        neighbor1Ix = 0;
      if (neighbor2Ix >= numNeighbors)
        neighbor2Ix = 1;
    }
    // ImGui call should be first otherwise when other condition is true widget is not drawn
    if (ImGui::DragInt("neigh1", &neighbor1Ix, 1, 0, numNeighbors - 1, "%d", ImGuiSliderFlags_None) || hasVertexChanged)
      n1Pos = ws::getOMeshVertexPosition(*oMesh, neighborIndices[neighbor1Ix]);
    if (ImGui::DragInt("neigh2", &neighbor2Ix, 1, 0, numNeighbors - 1, "%d", ImGuiSliderFlags_None) || hasVertexChanged)
      n2Pos = ws::getOMeshVertexPosition(*oMesh, neighborIndices[neighbor2Ix]);
    ImGui::Text("v: (%+.2f, %+.2f, %+.2f), n1: (%+.2f, %+.2f, %+.2f), n2: (%+.2f, %+.2f, %+.2f)",
                vPos.x, vPos.y, vPos.z,
                n1Pos.x, n1Pos.y, n1Pos.z,
                n2Pos.x, n2Pos.y, n2Pos.z);
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
