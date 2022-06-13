#pragma once

#include <glm/fwd.hpp>

#include <vector>

namespace OpenMesh
{
  class DefaultTraits;

  template <class>
  class TriMesh_ArrayKernelT;
}

namespace ws
{
  class Mesh;

  using OMesh = OpenMesh::TriMesh_ArrayKernelT<OpenMesh::DefaultTraits>;
  OMesh *makeIcosahedronOMesh();
  OMesh *makeIcosphereOMesh(uint32_t numSubDiv);
  Mesh *makeMeshFromOMesh(const OMesh &oMesh);

  uint32_t getOMeshNumVertices(const OMesh &oMesh);
  uint32_t getOMeshNumNeighbors(const OMesh &oMesh, int32_t ix);
  glm::vec3 getOMeshVertexPosition(const OMesh &oMesh, int32_t ix);
  std::vector<int> getOMeshVertexNeighborIndices(const OMesh &oMesh, int32_t ix);
  void splitOMeshVertex(OMesh &oMesh, int32_t vIx, int32_t n1Ix, int32_t n2Ix);
}