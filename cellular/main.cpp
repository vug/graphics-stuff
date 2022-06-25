#include "Cell.h"

#include <App.h>
#include <Shader.h>
#include <Mesh.h>
#include <OMesh.h>
#include <Camera.h>

#include <glad/gl.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <limits>
#include <memory>
#include <random>
#include <string>

class MyApp : public ws::App
{
public:
  // Because these don't have default constructors, can't make them members in class scope
  std::unique_ptr<ws::Shader> mainShader;
  std::unique_ptr<ws::Shader> pointShader;
  ws::OMesh *oMesh;
  std::unique_ptr<ws::Mesh> mesh;
  std::unique_ptr<ws::Mesh> meshSelectionViz;
  std::unique_ptr<ws::Camera> camera;
  std::mt19937 rng;
  std::uniform_real_distribution<float> dist;

  Specs getSpecs() final
  {
    return {.name = "MyApp", .width = 800u, .height = 600u, .shouldDebugOpenGL = true};
  }

  void onInit() final
  {
    rng = std::mt19937{std::random_device{}()};
    dist = std::uniform_real_distribution<float>();

    const char *mainShaderVertex = R"(
#version 460 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec2 vUV;
layout (location = 3) in vec4 vColor;

uniform mat4 WorldFromObject;
uniform mat4 ViewFromWorld;
uniform mat4 ProjectionFromView;

out VertexData
{
  vec3 position;
  vec3 normal;
  vec2 uv;
  vec4 color;
} vertexData;

void main()
{
  gl_Position = ProjectionFromView * ViewFromWorld * WorldFromObject * vec4(vPos, 1.0);
  gl_PointSize = 10.0f;

  vertexData.position = vPos;
  vertexData.normal = vNorm;
  vertexData.uv = vUV;
  vertexData.color = vColor;
}
)";

    const char *mainShaderFragment = R"(
#version 460 core

out vec4 FragColor;

void main()
{
  FragColor = gl_FrontFacing ? vec4(1.0f, 0.5f, 0.2f, 1.0f) : vec4(0.5f, 0.25f, 0.1f, 1.0f);
}
)";

    const char *pointShaderFragment = R"(
#version 460 core

in VertexData
{
  vec3 position;
  vec3 normal;
  vec2 uv;
  vec4 color;
} vertexData;

out vec4 FragColor;

void main()
{
  FragColor = vertexData.color;
}
)";
    mainShader = std::make_unique<ws::Shader>(mainShaderVertex, mainShaderFragment);
    pointShader = std::make_unique<ws::Shader>(mainShaderVertex, pointShaderFragment);

    oMesh = ws::makeIcosphereOMesh(1);
    // oMesh = ws::loadOMeshFromObjFile("../../assets/models/pentagon.obj");
    // oMesh = ws::makeDiskOMesh(5);
    mesh.reset(ws::makeMeshFromOMesh(*oMesh));

    camera = std::make_unique<ws::Camera>(static_cast<float>(specs.width), static_cast<float>(specs.height));

    meshSelectionViz = std::make_unique<ws::Mesh>(3);
    meshSelectionViz->verts[0].color = {1, 0, 0, 1};
    meshSelectionViz->verts[1].color = {0, 1, 0, 1};
    meshSelectionViz->verts[2].color = {0, 0, 1, 1};
    meshSelectionViz->idxs = {0, 1, 2};

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    // glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
  }

  void onRender(float time, float deltaTime) final
  {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static bool showDemo = false;
    static bool shouldOrbitCamera = false;
    static bool shouldAutoZoomCamera = true;
    ImGui::Begin("Main");
    ImGui::Text("FPS: %.0f", 1.f / deltaTime);
    ImGui::Checkbox("Orbit Camera", &shouldOrbitCamera);
    ImGui::Checkbox("Auto-zoom Camera", &shouldAutoZoomCamera);
    ImGui::Separator();

    // Simulation Parameters

    ImGui::DragFloat("linkRestLength", &cellular::parameters.linkRestLength, 0.01f, 0.f, 2.0f, "%.3f");
    ImGui::DragFloat("springFactor", &cellular::parameters.springFactor, 0.01f, 0.f, 2.0f, "%.3f");
    ImGui::DragFloat("planarFactor", &cellular::parameters.planarFactor, 0.01f, 0.f, 2.0f, "%.3f");
    ImGui::DragFloat("bulgeFactor", &cellular::parameters.bulgeFactor, 0.01f, 0.f, 2.0f, "%.3f");
    ImGui::DragFloat("radiusOfInfluence", &cellular::parameters.radiusOfInfluence, 0.01f, 0.f, 2.0f, "%.3f");
    ImGui::DragFloat("repulsionStrength", &cellular::parameters.repulsionStrength, 0.01f, 0.f, 2.0f, "%.3f");

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
    ImGui::Text("num cells: %d (%d)", mesh->verts.size(), numVerts);
    ImGui::Text("v: (%+.2f, %+.2f, %+.2f), n1: (%+.2f, %+.2f, %+.2f), n2: (%+.2f, %+.2f, %+.2f)",
                vPos.x, vPos.y, vPos.z,
                n1Pos.x, n1Pos.y, n1Pos.z,
                n2Pos.x, n2Pos.y, n2Pos.z);
    if (ImGui::Button("Split!"))
    {
      ws::splitOMeshVertex(*oMesh, vertexNo, neighbor1Ix, neighbor2Ix);
      ws::updateMeshFromOMesh(*mesh, *oMesh);
    }
    ImGui::Separator();

    // Random Split
    static int rndVertNo = 0;
    static int nRndSplits = 1;
    ImGui::DragInt("Num Splits", &nRndSplits, 1, 0, 1000);
    if (ImGui::Button("Random Split"))
    {
      for (int i = 0; i < nRndSplits; ++i)
      {
        rndVertNo = static_cast<int>(dist(rng) * numVerts);
        int nNeigh = ws::getOMeshNumNeighbors(*oMesh, rndVertNo);
        int n1 = static_cast<int>(dist(rng) * nNeigh);
        int n2 = (n1 + nNeigh / 2) % nNeigh;
        ws::splitOMeshVertex(*oMesh, rndVertNo, n1, n2);
      }
      ws::updateMeshFromOMesh(*mesh, *oMesh);
    }
    ImGui::Text("Random no: %d", rndVertNo);
    if (ImGui::Button("Save OBJ"))
      ws::saveOMeshToObjFile(*oMesh, "random_splits.obj");
    ImGui::Separator();

    static int numCorners = 5;
    ImGui::DragInt("Disk Corners", &numCorners, 1, 0, 8, "%d", ImGuiSliderFlags_None);
    if (ImGui::Button("Generate"))
    {
      delete oMesh;
      oMesh = ws::makeDiskOMesh(numCorners);
      ws::updateMeshFromOMesh(*mesh, *oMesh);
    }
    ImGui::Separator();

    // Simulation
    if (ImGui::Button("Update Positions!"))
    {
      cellular::updateCellPositions(*oMesh);
      ws::updateMeshFromOMesh(*mesh, *oMesh);
    }
    static bool shouldUpdatePositionsEveryFrame = false;
    ImGui::Checkbox("Auto-update positions", &shouldUpdatePositionsEveryFrame);
    ImGui::Separator();
    if (shouldUpdatePositionsEveryFrame)
    {
      cellular::updateCellPositions(*oMesh);
      ws::updateMeshFromOMesh(*mesh, *oMesh);
    }

    ImGui::Checkbox("Demo", &showDemo);
    ImGui::End();
    if (showDemo)
      ImGui::ShowDemoWindow();

    if (shouldOrbitCamera)
      camera->position = glm::rotate(camera->position, 1.0f * deltaTime, camera->UP);

    // zoom-out camera to keep all cells in view
    if (shouldAutoZoomCamera)
    {
      glm::vec3 minCoords{std::numeric_limits<float>::max()};
      glm::vec3 maxCoords{std::numeric_limits<float>::min()};
      for (const auto &v : mesh->verts)
      {
        if (v.position.x < minCoords.x)
          minCoords.x = v.position.x;
        if (v.position.y < minCoords.y)
          minCoords.y = v.position.y;
        if (v.position.z < minCoords.z)
          minCoords.z = v.position.z;
        if (v.position.x > maxCoords.x)
          maxCoords.x = v.position.x;
        if (v.position.y > maxCoords.y)
          maxCoords.y = v.position.y;
        if (v.position.z > maxCoords.z)
          maxCoords.z = v.position.z;
      }
      float diag = glm::distance(minCoords, maxCoords);
      float camDist = diag / glm::radians(camera->fov);
      camera->position = glm::normalize(camera->position) * camDist;
    }

    glUseProgram(mainShader->getId());
    mainShader->setMatrix4fv("ViewFromWorld", glm::value_ptr(camera->getViewFromWorld()));
    mainShader->setMatrix4fv("ProjectionFromView", glm::value_ptr(camera->getProjectionFromView()));

    glBindVertexArray(mesh->vao);
    mainShader->setMatrix4fv("WorldFromObject", glm::value_ptr(glm::mat4(1.f)));
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->idxs.size()), GL_UNSIGNED_INT, 0);

    glUseProgram(pointShader->getId());
    mainShader->setMatrix4fv("ViewFromWorld", glm::value_ptr(camera->getViewFromWorld()));
    mainShader->setMatrix4fv("ProjectionFromView", glm::value_ptr(camera->getProjectionFromView()));
    mainShader->setMatrix4fv("WorldFromObject", glm::value_ptr(glm::mat4(1.f)));

    meshSelectionViz->verts[0].position = vPos;
    meshSelectionViz->verts[1].position = n1Pos;
    meshSelectionViz->verts[2].position = n2Pos;
    meshSelectionViz->uploadData();
    glBindVertexArray(meshSelectionViz->vao);
    glDrawElements(GL_POINTS, static_cast<GLsizei>(meshSelectionViz->idxs.size()), GL_UNSIGNED_INT, 0);
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
