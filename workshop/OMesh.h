#pragma once

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/LoopT.hh>

namespace ws
{
  class Mesh;

  using OMesh = OpenMesh::TriMesh_ArrayKernelT<OpenMesh::DefaultTraits>;
  OMesh *makeIcosahedronOMesh();
  OMesh *makeIcosphereOMesh(uint32_t numSubDiv);
  Mesh *makeMeshFromOMesh(const OMesh &oMesh);
}