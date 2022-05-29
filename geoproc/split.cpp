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

  MyMesh::VertexHandle newVertex = mesh.add_vertex(MyMesh::Point(newX, newY, 0));
  mesh.vertex_split(newVertex, center, vertices[0], vertices[numCorners / 2]);
  OpenMesh::IO::write_mesh(mesh, filename);

  return 0;
}