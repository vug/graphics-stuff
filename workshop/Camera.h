#pragma once

#include <glm/vec3.hpp>
#include <glm/fwd.hpp>

namespace ws
{
  class Camera
  {
  public:
    glm::vec3 position = {0.f, .2f, 5.f};
    glm::vec3 target = {0, 0, 0};
    static const glm::vec3 UP;
    float fov = 45.f;
    float width = 1.f;
    float height = 1.f;

    Camera() = default;
    Camera(float width, float height);

    glm::mat4 getViewFromWorld() const;
    glm::mat4 getProjectionFromView() const;
  };
}