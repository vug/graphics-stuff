#pragma once
#include "../OMesh.h"

namespace cellular
{
  struct Parameters
  {
    float linkRestLength = 2.11f;
    float springFactor = 0.4f;
    float planarFactor = 0.2348f;
    float bulgeFactor = 0.1f;
    float radiusOfInfluence = 1.5f;
    float repulsionStrength = 0.26f;
  };

  glm::vec3 newCellPosition(ws::OMesh &oMesh, int32_t vIx);

  void updateCellPositions(ws::OMesh &oMesh);
}