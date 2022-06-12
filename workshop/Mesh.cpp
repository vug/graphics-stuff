#include "Mesh.h"

#include <glad/gl.h>
#include <OpenMesh/Tools/Subdivider/Uniform/LoopT.hh>

#include <cmath>

namespace ws
{
  Mesh::Mesh(size_t capacity)
      : capacity(capacity)
  {
    verts.reserve(capacity);
    idxs.reserve(capacity);

    createBuffers();
    uploadData();
  }

  Mesh::Mesh(const std::vector<DefaultVertex> &vertices, const std::vector<uint32_t> &indices)
      : verts(std::move(vertices)), idxs(std::move(indices))
  {
    size_t cnt = vertices.size();
    // next power of two larger than size
    capacity = static_cast<size_t>(std::pow(2.0, std::ceil(std::log2(cnt))));

    createBuffers();
    uploadData();
  }

  Mesh::~Mesh()
  {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
  }

  void Mesh::createBuffers()
  {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    allocateBuffers();

    static const std::vector<int32_t> sizes = {3, 3, 2, 4, 4, 4};
    size_t offset = 0;
    for (int ix = 0; ix < sizes.size(); ++ix)
    {
      glVertexAttribPointer(ix, sizes[ix], GL_FLOAT, GL_FALSE, sizeof(DefaultVertex), (void *)offset);
      glEnableVertexAttribArray(ix);
      offset += sizes[ix] * sizeof(float);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  void Mesh::allocateBuffers()
  {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(DefaultVertex) * capacity, nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * capacity, nullptr, GL_DYNAMIC_DRAW);
  }

  void Mesh::uploadData()
  {
    if (capacity < idxs.size())
    {
      do
      {
        capacity *= 2;
      } while (capacity <= idxs.size());

      allocateBuffers();
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(DefaultVertex) * verts.size(), verts.data());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * idxs.size(), idxs.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  // ---------
  void makeIcosahedronOMesh(OMesh &oMesh)
  {

    float phi = (1.0f + sqrt(5.0f)) * 0.5f; // golden ratio
    float a = 1.0f;
    float b = 1.0f / phi;

    std::vector<OMesh::Point> points = {
        {0, b, -a},
        {b, a, 0},
        {-b, a, 0},
        {0, b, a},
        {0, -b, a},
        {-a, 0, b},
        {0, -b, -a},
        {a, 0, -b},
        {a, 0, b},
        {-a, 0, -b},
        {b, -a, 0},
        {-b, -a, 0},
    };

    std::vector<OMesh::VertexHandle> vertices;
    for (const auto &p : points)
      vertices.push_back(oMesh.add_vertex(p / p.norm()));

    std::vector<std::vector<OMesh::VertexHandle>> faceTriangles = {
        {vertices[2], vertices[1], vertices[0]},   // 0
        {vertices[1], vertices[2], vertices[3]},   // 1
        {vertices[5], vertices[4], vertices[3]},   //
        {vertices[4], vertices[8], vertices[3]},   //
        {vertices[7], vertices[6], vertices[0]},   //
        {vertices[6], vertices[9], vertices[0]},   //
        {vertices[11], vertices[10], vertices[4]}, //
        {vertices[10], vertices[11], vertices[6]}, //
        {vertices[9], vertices[5], vertices[2]},   //
        {vertices[5], vertices[9], vertices[11]},  //
        {vertices[8], vertices[7], vertices[1]},   //
        {vertices[7], vertices[8], vertices[10]},  //
        {vertices[2], vertices[5], vertices[3]},   //
        {vertices[8], vertices[1], vertices[3]},   //
        {vertices[9], vertices[2], vertices[0]},   //
        {vertices[1], vertices[7], vertices[0]},   //
        {vertices[11], vertices[9], vertices[6]},  //
        {vertices[7], vertices[10], vertices[6]},  //
        {vertices[5], vertices[11], vertices[4]},  //
        {vertices[10], vertices[8], vertices[4]},  //
    };

    for (const auto &f : faceTriangles)
      oMesh.add_face(f);
  }

  void makeIcosphereOMesh(OMesh &oMesh, uint32_t numSubDiv)
  {
    makeIcosahedronOMesh(oMesh);

    if (numSubDiv == 0)
      return;

    // At each iteration first subdivide then project on sphere instead of subdividing n-times and projecting at the end
    for (uint32_t i = 0; i < numSubDiv; ++i)
    {
      OpenMesh::Subdivider::Uniform::LoopT<OMesh> loopSubd;
      loopSubd.attach(oMesh);
      loopSubd(1);
      loopSubd.detach();

      for (auto &v : oMesh.vertices())
      {
        auto &p = oMesh.point(v);
        p.normalize();
      }
    }
  }

  void makeMeshFromOMesh(const OMesh &oMesh, std::vector<DefaultVertex> &vertices, std::vector<uint32_t> &indices)
  {
    vertices.clear();
    indices.clear();

    for (const auto &v : oMesh.vertices())
    {
      const auto &p = oMesh.point(v);
      vertices.emplace_back(DefaultVertex{{p[0], p[1], p[2]}});
      const auto &v = vertices[vertices.size() - 1];
    }

    for (auto f : oMesh.faces())
    {
      // const auto& fNorm = oMesh.calc_normal(f);
      for (auto v : f.vertices())
      {
        const int ix = v.idx();
        indices.push_back(v.idx());

        // flat shading for normals: won't work because vertices are shared among faces
        // glm::vec3 n = {fNorm[0], fNorm[1], fNorm[2]};

        const auto &vNorm = oMesh.calc_normal(v);
        const glm::vec3 n = {vNorm[0], vNorm[1], vNorm[2]};

        // cheat for sphere
        // const auto& p = oMesh.point(v);
        // glm::vec3 n = {p[0], p[1], p[2]};

        vertices[ix].normal = n;
      }
    }
  }
}