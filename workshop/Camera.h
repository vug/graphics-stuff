#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/fwd.hpp>

namespace ws
{
  class Camera
  {
  public:
    float width{1.f};
    float height{1.f};

    Camera() = default;
    Camera(const float width, const float height);

    virtual glm::mat4 getViewFromWorld() const = 0;
    virtual glm::mat4 getProjectionFromView() const = 0;
  };

  class CameraPerspective : public Camera
  {
  public:
    glm::vec3 position{};
    glm::vec3 target{0, 0, 0};
    static const glm::vec3 UP;
    float fov{45.f};

    CameraPerspective(const float width, const float height, const glm::vec3 &position = {0.f, .2f, 5.f});

    glm::mat4 getViewFromWorld() const final;
    glm::mat4 getProjectionFromView() const final;
  };

  class Camera2D : public Camera
  {
  public:
    glm::vec2 position{};
    Camera2D(const float width, const float height, const glm::vec2 &position = {});

    glm::mat4 getViewFromWorld() const final;
    glm::mat4 getProjectionFromView() const final;
  };
}