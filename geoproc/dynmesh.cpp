#include <raylib.h>
#include <rlgl.h>

#include <iostream>

// Utility function to prepare a mesh made of n triangles
// only fills vertices and indices. rest of the member are not used.
void makeTriangleMesh(Mesh &mesh, int n)
{
  mesh.triangleCount = n;
  mesh.vertexCount = n * 3;
  mesh.vertices = new float[mesh.vertexCount * 3];
  mesh.indices = new unsigned short[mesh.vertexCount];

  for (int i = 0; i < n; ++i)
  {
    float triangle[9] = {0.f, 0.f, static_cast<float>(i), 1.f, 0.f, static_cast<float>(i), 0.f, 1.f, static_cast<float>(i)};
    for (int j = 0; j < 9; ++j)
      mesh.vertices[i * 9 + j] = triangle[j];
  }
  for (unsigned short ix = 0; ix < n * 3; ++ix)
    mesh.indices[ix] = ix;
}

void debugPrint(Model &model)
{
  std::cout << "vertices address: " << model.meshes[0].vertices << "\n";
  std::cout << "vaoId: " << model.meshes[0].vaoId << ", vboId (vert): " << model.meshes[0].vboId[0] << ", vboId (ind): " << model.meshes[0].vboId[6] << "\n";
  for (int i = 0; i < model.meshes[0].vertexCount; ++i)
    std::cout << model.meshes[0].indices[i] << " ";
  std::cout << "\n";
}

int main()
{
  Mesh mesh{};
  Model model;
  int numTriangles = 1;

  InitWindow(800, 600, "Dynamic Mesh test");
  SetTargetFPS(60);
  rlDisableBackfaceCulling();
  rlEnableSmoothLines();
  Camera camera = {{5.0f, 5.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f, 0};
  SetCameraMode(camera, CAMERA_ORBITAL);

  makeTriangleMesh(mesh, numTriangles); // start with 1 triangle
  UploadMesh(&mesh, true);
  model = LoadModelFromMesh(mesh);
  debugPrint(model);

  while (!WindowShouldClose()) // ESC
  {
    if (IsKeyPressed('T')) // Method 1
    {
      UnloadModel(model);
      mesh = {};
      makeTriangleMesh(mesh, ++numTriangles);
      UploadMesh(&mesh, true);
      model = LoadModelFromMesh(mesh);
      debugPrint(model);
    }
    if (IsKeyPressed('Y')) // Method 2
    {
      Mesh &modelMesh = model.meshes[0];
      makeTriangleMesh(modelMesh, ++numTriangles);
      UpdateMeshBuffer(modelMesh, 0, modelMesh.vertices, sizeof(float) * modelMesh.vertexCount * 3, 0);
      // from UploadMesh: index of vbo for indices is 6
      UpdateMeshBuffer(modelMesh, 6, modelMesh.indices, sizeof(unsigned int) * modelMesh.vertexCount, 0);
      debugPrint(model);
    }

    UpdateCamera(&camera); // Update internal camera and our camera

    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);
    DrawModelWires(model, {0.0f, 0.0f, 0.0f}, 1.0f, RED);
    DrawGrid(10, 1.0);
    EndMode3D();
    EndDrawing();
  }

  UnloadModel(model);
  CloseWindow();
  return 0;
}