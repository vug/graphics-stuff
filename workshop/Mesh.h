#pragma once

#include <glm/glm.hpp>

#include <vector>

constexpr uint32_t INVALID = static_cast<uint32_t>(-1);

namespace ws
{
  struct DefaultVertex
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec4 color;
    glm::vec4 custom1;
    glm::vec4 custom2;
  };

  // https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
  class Mesh
  {
  public:
    Mesh(size_t capacity);
    Mesh(const std::vector<DefaultVertex> &vertices, const std::vector<uint32_t> &indices);

    std::vector<DefaultVertex> verts;
    std::vector<uint32_t> idxs;
    size_t capacity{};

    uint32_t vao{INVALID};
    uint32_t vbo{INVALID};
    uint32_t ebo{INVALID};

  private:
    void createBuffers();
  };
}