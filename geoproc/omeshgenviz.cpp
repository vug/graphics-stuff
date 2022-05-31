#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <raylib.h>
#include <rlgl.h>

#include <cmath>
#include <numbers>
#include <vector>

#include <iostream>

using MyMesh = OpenMesh::TriMesh_ArrayKernelT<>;

void convertOpenMeshToRaylibMesh(const MyMesh &oMesh, Mesh &rMesh)
{
  if (rMesh.vertices)
  {
    // delete[] rMesh.vertices;
    // delete[] rMesh.indices;
    UnloadMesh(rMesh);
    rMesh = {};
  }

  rMesh.triangleCount = static_cast<int>(oMesh.n_faces());
  rMesh.vertexCount = static_cast<int>(oMesh.n_vertices());
  rMesh.vertices = new float[rMesh.vertexCount * 3];     // 3 vertices, 3 coordinates each (x, y, z)
  rMesh.indices = new unsigned short[rMesh.vertexCount]; // 3 vertices, 3 coordinates each (x, y, z)
  // rMesh.texcoords = (float *)MemAlloc(rMesh.vertexCount * 2 * sizeof(float));             // 3 vertices, 2 coordinates each (x, y)
  // rMesh.normals = (float *)MemAlloc(rMesh.vertexCount * 3 * sizeof(float));               // 3 vertices, 3 coordinates each (x, y, z)

  size_t ix = 0;
  for (auto v : oMesh.vertices())
  {
    auto p = oMesh.point(v);
    OpenMesh::DefaultTraits::Point q;
    rMesh.vertices[ix++] = p[0];
    rMesh.vertices[ix++] = p[1];
    rMesh.vertices[ix++] = p[2];
  }
  ix = 0;
  for (auto f : oMesh.faces())
    for (auto v : f.vertices())
      rMesh.indices[ix++] = static_cast<unsigned short>(v.idx());

  UploadMesh(&rMesh, true);
}

int main()
{
  MyMesh oMesh;
  Mesh rMesh{};
  Model model;

  std::vector<MyMesh::VertexHandle> vertices;
  const int numCorners = 6;
  for (int i = 0; i < numCorners; ++i)
  {
    const double x = std::cos(2.0 * std::numbers::pi * i / numCorners);
    const double y = std::sin(2.0 * std::numbers::pi * i / numCorners);
    vertices.push_back(oMesh.add_vertex(MyMesh::Point(x, y, 0)));
  }
  const auto &center = oMesh.add_vertex(MyMesh::Point(0, 0, 0));
  vertices.push_back(center);
  for (int i = 0; i < numCorners; ++i)
  {
    std::vector<MyMesh::VertexHandle> face = {center, vertices[i], vertices[(i + 1) % numCorners]};
    oMesh.add_face(face);
  }

  const int screenWidth = 800;
  const int screenHeight = 600;
  InitWindow(screenWidth, screenHeight, "OpenMesh Visualization");
  SetTargetFPS(60);

  convertOpenMeshToRaylibMesh(oMesh, rMesh);
  model = LoadModelFromMesh(rMesh);

  rlDisableBackfaceCulling();
  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  const Vector3 camPos = {5.0f, 5.0f, 5.0f};
  const Vector3 camTarget = {0.0f, 0.0f, 0.0f};
  Camera camera = {camPos, camTarget, {0.0f, 1.0f, 0.0f}, 45.0f, 0};
  Vector3 position = {0.0f, 0.0f, 0.0f};
  SetCameraMode(camera, CAMERA_FREE); // Set a orbital camera mode
  SetCameraPanControl(MOUSE_BUTTON_LEFT);

  while (!WindowShouldClose()) // ESC
  {
    if (IsKeyDown('Z')) // Focus camera
    {
      camera.position = camPos;
      camera.target = camTarget;
    }
    if (IsKeyPressed('S'))
    {
      // Only do the split if vl and vr are neighbors of v (otw OpenMesh silently crashes)
      auto v = center;
      auto vl = vertices[0];
      auto vr = vertices[numCorners / 2];
      bool isLeftNeighbor = false;
      bool isRightNeighbor = false;
      for (auto vv_it = oMesh.vv_iter(v); vv_it.is_valid(); ++vv_it)
      {
        if (*vv_it == vl)
          isLeftNeighbor = true;
        if (*vv_it == vr)
          isRightNeighbor = true;
      }
      MyMesh::VertexHandle newVertex = oMesh.add_vertex(MyMesh::Point(0.0, 0.5, 0));
      if (isLeftNeighbor && isRightNeighbor)
        oMesh.vertex_split(newVertex, v, vl, vr);

      // convertOpenMeshToRaylibMesh(oMesh, rMesh);
      // model = LoadModelFromMesh(rMesh);
    }

    UpdateCamera(&camera); // Update internal camera and our camera

    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);
    DrawModelWires(model, position, 1.0f, RED);
    DrawGrid(10, 1.0);
    EndMode3D();
    EndDrawing();
  }

  // OpenMesh::IO::write_mesh(mesh, filename);

  std::cout << "Bye!\n";
  CloseWindow(); // Close window and OpenGL context
  return 0;
}