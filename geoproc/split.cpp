#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include <cmath>
#include <numbers>
#include <vector>

using MyMesh = OpenMesh::TriMesh_ArrayKernelT<>;

int main(int argc, char *argv[])
{
  const char *filename = argc >= 2 ? argv[1] : "split_splitted.off";
  double newX = 0.0;
  double newY = 0.25;
  if (argc == 4)
  {
    newX = atof(argv[2]);
    newY = atof(argv[3]);
  }

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
  OpenMesh::IO::write_mesh(mesh, "split_original.off");

  // Only do the split if vl and vr are neighbors of v (otw OpenMesh silently crashes)
  auto v = center;
  auto vl = vertices[0];
  auto vr = vertices[numCorners / 2];
  bool isLeftNeighbor = false;
  bool isRightNeighbor = false;
  for (auto vv_it = mesh.vv_iter(v); vv_it.is_valid(); ++vv_it)
  {
    if (*vv_it == vl)
      isLeftNeighbor = true;
    if (*vv_it == vr)
      isRightNeighbor = true;
  }
  MyMesh::VertexHandle newVertex = mesh.add_vertex(MyMesh::Point(newX, newY, 0));
  if (isLeftNeighbor && isRightNeighbor)
    mesh.vertex_split(newVertex, v, vl, vr);
  OpenMesh::IO::write_mesh(mesh, filename);

  return 0;
}