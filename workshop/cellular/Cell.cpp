#include "Cell.h"

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/LoopT.hh>

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/norm.hpp>

namespace cellular
{
  Parameters parameters;

  glm::vec3 newCellPosition(ws::OMesh &oMesh, int32_t vIx)
  {
    const ws::OMesh::VertexHandle vh{vIx};
    const ws::OMesh::Point pp = oMesh.point(vh);
    glm::vec3 vp{pp[0], pp[1], pp[2]};

    glm::vec3 springTarget = {};
    glm::vec3 planarTarget = {};
    glm::vec3 collisionOffset = {};

    float bulgeDist = 0.0f;
    const ws::OMesh::Normal np = oMesh.calc_normal(vh);
    glm::vec3 normal{np[0], np[1], np[2]};
    uint32_t numNeighbors = 0;
    for (auto vv_it = oMesh.cvv_cwiter(vh); vv_it.is_valid(); ++vv_it)
    {
      ++numNeighbors;
      const ws::OMesh::Point qp = oMesh.point(*vv_it);
      glm::vec3 q{qp[0], qp[1], qp[2]};
      glm::vec3 d = q - vp;
      springTarget += q + parameters.linkRestLength * -glm::normalize(d);
      planarTarget += q;

      float dSqr = glm::length2(d);
      float rSqr = parameters.linkRestLength * parameters.linkRestLength;
      if (dSqr < rSqr) // can't push if too far away
      {
        float dot = glm::dot(d, normal);
        bulgeDist += glm::sqrt(rSqr - dSqr + dot * dot) + dot;
      }
    }
    float roi2 = parameters.radiusOfInfluence * parameters.radiusOfInfluence;

    for (auto uh : oMesh.vertices())
    {
      const ws::OMesh::Point upp = oMesh.point(uh);
      glm::vec3 up{upp[0], upp[1], upp[2]};

      if (uh.idx() == vh.idx() || oMesh.find_halfedge(vh, uh).is_valid() || glm::length2(up - vp) > roi2)
      {
        continue;
      }
      glm::vec3 d = vp - up;
      collisionOffset += (1 - glm::length2(d) / roi2) * glm::normalize(d);
    }
    springTarget /= numNeighbors;
    planarTarget /= numNeighbors;
    glm::vec3 bulgeTarget = vp + normal * (bulgeDist / numNeighbors);

    vp = vp + parameters.springFactor * (springTarget - vp) + parameters.planarFactor * (planarTarget - vp) + parameters.bulgeFactor * (bulgeTarget - vp) + parameters.repulsionStrength * collisionOffset;
    return vp;
  }

  void updateCellPositions(ws::OMesh &oMesh)
  {
    std::vector<glm::vec3> newPositions{oMesh.n_vertices()};

    for (const auto &vh : oMesh.vertices())
      newPositions[vh.idx()] = newCellPosition(oMesh, vh.idx());

    for (auto &vh : oMesh.vertices())
    {
      const glm::vec3 v = newPositions[vh.idx()];
      oMesh.point(vh) = {v.x, v.y, v.z};
      // oMesh.set_point(vh, {v.x, v.y, v.z});
    }

    oMesh.update_normals();
  }
}