#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/LoopT.hh>

#include <vector>

constexpr uint32_t INVALID = static_cast<uint32_t>(-1);

namespace ws
{
  struct DefaultVertex
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec4 color;
    glm::vec4 custom1;
    glm::vec4 custom2;
  };

  // https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
  class Mesh
  {
  public:
    Mesh(size_t capacity);
    Mesh(const std::vector<DefaultVertex> &vertices, const std::vector<uint32_t> &indices);
    ~Mesh();

    std::vector<DefaultVertex> verts;
    std::vector<uint32_t> idxs;
    size_t capacity{};

    uint32_t vao{INVALID};
    uint32_t vbo{INVALID};
    uint32_t ebo{INVALID};

    // call after setting verts and idxs to upload them to GPU
    void uploadData();

  private:
    void createBuffers();
    void allocateBuffers();
  };

  using OMesh = OpenMesh::TriMesh_ArrayKernelT<>;
  void makeIcosahedronOMesh(OMesh &oMesh);
  void makeIcosphereOMesh(OMesh &oMesh, uint32_t numSubDiv);
  void makeMeshFromOMesh(const OMesh &oMesh, std::vector<DefaultVertex> &vertices, std::vector<uint32_t> &indices);
}