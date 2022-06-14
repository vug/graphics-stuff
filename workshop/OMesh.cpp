#include "OMesh.h"

#include "Mesh.h"

#include <glm/vec3.hpp>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/LoopT.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>

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

  OMesh *makeDiskOMesh(uint32_t numCorners)
  {
    OMesh *oMesh = new OMesh();
    OMesh::VertexHandle center = oMesh->add_vertex({0, 0, 0});

    std::vector<OMesh::VertexHandle> corners;
    for (uint32_t i = 0; i < numCorners; ++i)
    {
      float angle = 2 * M_PI * i / numCorners;
      corners.push_back(oMesh->add_vertex({std::cos(angle), std::sin(angle), 0}));
    }

    std::vector<std::vector<OMesh::VertexHandle>> triangles;
    for (uint32_t i = 0; i < numCorners; ++i)
    {
      uint32_t j = (i + 1) % numCorners;
      triangles.push_back({center, corners[i], corners[j]});
    }

    for (const auto &f : triangles)
      oMesh->add_face(f);

    return oMesh;
  }

  OMesh *loadOMeshFromObjFile(const char *filepath)
  {
    OMesh *oMesh = new OMesh();

    if (!OpenMesh::IO::read_mesh(*oMesh, filepath))
    {
      std::cerr << "error reading " << filepath << "\n ";
      exit(1);
    }
    return oMesh;
  }

  void saveOMeshToObjFile(const OMesh &oMesh, const char *filepath)
  {
    if (!OpenMesh::IO::write_mesh(oMesh, filepath))
    {
      std::cerr << "error writing " << filepath << "\n ";
      exit(1);
    }
  }

  void updateMeshFromOMesh(Mesh &mesh, const OMesh &oMesh)
  {
    mesh.verts.clear();
    mesh.idxs.clear();

    for (const auto &v : oMesh.vertices())
    {
      const auto &p = oMesh.point(v);
      mesh.verts.emplace_back(DefaultVertex{{p[0], p[1], p[2]}});
      const auto &v = mesh.verts[mesh.verts.size() - 1];
    }

    for (auto f : oMesh.faces())
    {
      // const auto& fNorm = oMesh.calc_normal(f);
      for (auto v : f.vertices())
      {
        const int ix = v.idx();
        mesh.idxs.push_back(v.idx());

        // flat shading for normals: won't work because vertices are shared among faces
        // glm::vec3 n = {fNorm[0], fNorm[1], fNorm[2]};

        const auto &vNorm = oMesh.calc_normal(v);
        const glm::vec3 n = {vNorm[0], vNorm[1], vNorm[2]};

        // cheat for sphere
        // const auto& p = oMesh.point(v);
        // glm::vec3 n = {p[0], p[1], p[2]};

        mesh.verts[ix].normal = n;
      }
    }

    mesh.uploadData();
  }

  Mesh *makeMeshFromOMesh(const OMesh &oMesh)
  {
    Mesh *mesh = new Mesh{1};
    updateMeshFromOMesh(*mesh, oMesh);
    return mesh;
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

  void splitOMeshVertex(OMesh &oMesh, int32_t vIx, int32_t n1Ix, int32_t n2Ix)
  {
    std::vector<int> idxs = getOMeshVertexNeighborIndices(oMesh, vIx);
    int n1hIx = idxs[n1Ix];
    int n2hIx = idxs[n2Ix];

    int i = 0;
    // (a) b n1 c d n2 e f
    while (idxs[i] != n1hIx)
    {
      idxs.push_back(idxs[i]);
      ++i;
    }
    // a b (n1) c d n2 e f a b
    // OMesh::Point p1 = {0, 0, 0};
    // int cnt1 = 0;
    OMesh::Point p1 = oMesh.point(OMesh::VertexHandle{vIx}) + oMesh.point(OMesh::VertexHandle{n2hIx});
    int cnt1 = 2;
    while (idxs[i] != n2hIx)
    {
      const OMesh::VertexHandle vh{idxs[i]};
      p1 += oMesh.point(vh);
      ++i;
      ++cnt1;
    }
    p1 /= cnt1; // p1 = (n1 + c + d) / 3
    // OMesh::Point p2 = {0, 0, 0};
    // int cnt2 = 0;
    OMesh::Point p2 = oMesh.point(OMesh::VertexHandle{vIx}) + oMesh.point(OMesh::VertexHandle{n1hIx});
    int cnt2 = 2;
    while (i < idxs.size())
    {
      const OMesh::VertexHandle vh{idxs[i]};
      p2 += oMesh.point(vh);
      ++i;
      ++cnt2;
    }
    p2 /= cnt2; // p2 = (n2 + e + f + a + b) / 5

    oMesh.set_point(OMesh::VertexHandle{vIx}, p1);
    OMesh::VertexHandle newVh = oMesh.add_vertex(p2);
    oMesh.vertex_split(newVh, OMesh::VertexHandle{vIx}, OMesh::VertexHandle{n1hIx}, OMesh::VertexHandle{n2hIx});
  }
}