#include "OMesh.h"

#include "Mesh.h"

#include <glm/vec3.hpp>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/LoopT.hh>

namespace ws
{
  OMesh *makeIcosahedronOMesh()
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

    OMesh *oMesh = new OMesh();
    std::vector<OMesh::VertexHandle> vertices;
    for (const auto &p : points)
      vertices.push_back(oMesh->add_vertex(p / p.norm()));

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
      oMesh->add_face(f);

    return oMesh;
  }

  OMesh *makeIcosphereOMesh(uint32_t numSubDiv)
  {
    OMesh *oMesh = makeIcosahedronOMesh();

    if (numSubDiv == 0)
      return oMesh;

    // At each iteration first subdivide then project on sphere instead of subdividing n-times and projecting at the end
    for (uint32_t i = 0; i < numSubDiv; ++i)
    {
      OpenMesh::Subdivider::Uniform::LoopT<OMesh> loopSubd;
      loopSubd.attach(*oMesh);
      loopSubd(1);
      loopSubd.detach();

      for (auto &v : oMesh->vertices())
      {
        auto &p = oMesh->point(v);
        p.normalize();
      }
    }

    return oMesh;
  }

  Mesh *makeMeshFromOMesh(const OMesh &oMesh)
  {
    std::vector<DefaultVertex> vertices;
    std::vector<uint32_t> indices;

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

    return new Mesh{vertices, indices};
  }

  uint32_t getOMeshNumVertices(const OMesh &oMesh)
  {
    return oMesh.n_vertices();
  }

  uint32_t getOMeshNumNeighbors(const OMesh &oMesh, int32_t ix)
  {
    const OMesh::VertexHandle vh{ix};
    uint32_t cnt = 0;
    for (auto vv_it = oMesh.cvv_cwiter(vh); vv_it.is_valid(); ++vv_it)
      cnt++;
    return cnt;
  }

  glm::vec3 getOMeshVertexPosition(const OMesh &oMesh, int32_t ix)
  {
    OMesh::VertexHandle vh{ix};
    auto p = oMesh.point(vh);
    return {p[0], p[1], p[2]};
  }

  std::vector<int> getOMeshVertexNeighborIndices(const OMesh &oMesh, int32_t ix)
  {
    const OMesh::VertexHandle vh{ix};
    std::vector<int> indices;
    for (auto vv_it = oMesh.cvv_cwiter(vh); vv_it.is_valid(); ++vv_it)
      indices.emplace_back(vv_it->idx());
    return indices;
  }
}