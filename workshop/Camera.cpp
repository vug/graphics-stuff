#include "Camera.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ws
{
  const glm::vec3 Camera::UP = {0.f, 1.f, 0.f};

  Camera::Camera(float width, float height)
      : width(width), height(height) {}

  glm::mat4 Camera::getViewFromWorld() const
  {
    return glm::lookAt(position, target, UP);
  }

  glm::mat4 Camera::getProjectionFromView() const
  {
    return glm::perspective(glm::radians(fov), width / height, 0.1f, 100.0f);
  }

}