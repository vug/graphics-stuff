#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <raylib.h>
#include <rlgl.h>

#include <cmath>
#include <numbers>
#include <vector>

using MyMesh = OpenMesh::TriMesh_ArrayKernelT<>;

int main()
{
  MyMesh mesh;
  std::vector<MyMesh::VertexHandle> vertices;
  const int numCorners = 6;
  for (int i = 0; i < numCorners; ++i)
  {
    const double x = std::cos(2.0 * std::numbers::pi * i / numCorners);
    const double y = std::sin(2.0 * std::numbers::pi * i / numCorners);
    vertices.push_back(mesh.add_vertex(MyMesh::Point(x, y, 0)));
  }
  const auto &center = mesh.add_vertex(MyMesh::Point(0, 0, 0));
  vertices.push_back(center);
  for (int i = 0; i < numCorners; ++i)
  {
    std::vector<MyMesh::VertexHandle> face = {center, vertices[i], vertices[(i + 1) % numCorners]};
    mesh.add_face(face);
  }

  const int screenWidth = 800;
  const int screenHeight = 600;
  InitWindow(screenWidth, screenHeight, "OpenMesh Visualization");
  SetTargetFPS(60);

  Mesh rMesh = {0};
  rMesh.triangleCount = static_cast<int>(mesh.n_faces());
  rMesh.vertexCount = static_cast<int>(mesh.n_vertices());
  rMesh.vertices = (float *)MemAlloc(rMesh.vertexCount * 3 * sizeof(float)); // 3 vertices, 3 coordinates each (x, y, z)
  // rMesh.texcoords = (float *)MemAlloc(rMesh.vertexCount * 2 * sizeof(float));             // 3 vertices, 2 coordinates each (x, y)
  // rMesh.normals = (float *)MemAlloc(rMesh.vertexCount * 3 * sizeof(float));               // 3 vertices, 3 coordinates each (x, y, z)
  rMesh.indices = (unsigned short *)MemAlloc(rMesh.vertexCount * sizeof(unsigned short)); // 3 vertices, 3 coordinates each (x, y, z)

  size_t ix = 0;
  for (auto v : mesh.vertices())
  {
    auto p = mesh.point(v);
    OpenMesh::DefaultTraits::Point q;
    rMesh.vertices[ix++] = p[0];
    rMesh.vertices[ix++] = p[1];
    rMesh.vertices[ix++] = p[2];
  }
  ix = 0;
  for (auto f : mesh.faces())
    for (auto v : f.vertices())
      rMesh.indices[ix++] = static_cast<unsigned short>(v.idx());
  UploadMesh(&rMesh, false);
  Model model = LoadModelFromMesh(rMesh);

  rlDisableBackfaceCulling();
  Camera camera = {{5.0f, 5.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 45.0f, 0};
  Vector3 position = {0.0f, 0.0f, 0.0f};
  SetCameraMode(camera, CAMERA_FREE); // Set a orbital camera mode
  SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second

  while (!WindowShouldClose()) // ESC
  {
    UpdateCamera(&camera); // Update internal camera and our camera
    BeginDrawing();

    ClearBackground(RAYWHITE);

    BeginMode3D(camera);

    DrawModelWires(model, position, 1.0f, RED);
    DrawGrid(10, 1.0);

    EndMode3D();

    EndDrawing();
  }

  // // Only do the split if vl and vr are neighbors of v (otw OpenMesh silently crashes)
  // auto v = center;
  // auto vl = vertices[0];
  // auto vr = vertices[numCorners / 2];
  // bool isLeftNeighbor = false;
  // bool isRightNeighbor = false;
  // for (auto vv_it = mesh.vv_iter(v); vv_it.is_valid(); ++vv_it)
  // {
  //   if (*vv_it == vl)
  //     isLeftNeighbor = true;
  //   if (*vv_it == vr)
  //     isRightNeighbor = true;
  // }
  // MyMesh::VertexHandle newVertex = mesh.add_vertex(MyMesh::Point(newX, newY, 0));
  // if (isLeftNeighbor && isRightNeighbor)
  //   mesh.vertex_split(newVertex, v, vl, vr);
  // OpenMesh::IO::write_mesh(mesh, filename);

  CloseWindow(); // Close window and OpenGL context
  return 0;
}