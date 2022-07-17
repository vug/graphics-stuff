#include "Camera.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ws
{
  const glm::vec3 CameraPerspective::UP = {0.f, 1.f, 0.f};

  Camera::Camera(const float width, const float height)
      : width(width), height(height) {}

  CameraPerspective::CameraPerspective(const float width, const float height, const glm::vec3 &position)
      : Camera(width, height), position(position) {}

  glm::mat4 CameraPerspective::getViewFromWorld() const
  {
    return glm::lookAt(position, target, UP);
  }

  glm::mat4 CameraPerspective::getProjectionFromView() const
  {
    return glm::perspective(glm::radians(fov), width / height, 0.1f, 100.0f);
  }

  Camera2D::Camera2D(const float width, const float height, const glm::vec2 &position)
      : Camera(width, height), position(position) {}

  glm::mat4 Camera2D::getViewFromWorld() const
  {
    return glm::mat4(1);
  }

  glm::mat4 Camera2D::getProjectionFromView() const
  {
    const float halfWidth = width * 0.5f;
    const float halfHeight = height * 0.5f;
    return glm::ortho(position.x - halfWidth, position.x + halfWidth, position.y - halfHeight, position.y + halfHeight, -1.f, 1.f);
  }

}