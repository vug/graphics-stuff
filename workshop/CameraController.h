#pragma once

#include "Camera.h"

namespace ws
{
  class Camera2DController
  {
  public:
    Camera2DController(Camera2D &cam2D);

    void update(float viewWidth, float viewHeight);

  private:
    Camera2D &camera;
  };
}