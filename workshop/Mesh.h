#pragma once

#include "Common.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>

namespace ws
{
  struct DefaultVertex
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec4 color = {1, 1, 1, 1};
    glm::vec4 custom1;
    glm::vec4 custom2;
  };

  // https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
  class Mesh
  {
  public:
    enum class Type
    {
      Points,
      Lines,
      Triangles,
    };

    Mesh(size_t capacity, Type type = Type::Triangles);
    Mesh(const std::vector<DefaultVertex> &vertices, const std::vector<uint32_t> &indices, Type type = Type::Triangles);
    ~Mesh();

    Type type = Type::Triangles;
    std::vector<DefaultVertex> verts;
    std::vector<uint32_t> idxs;
    size_t capacity{};

    uint32_t vao{INVALID};
    uint32_t vbo{INVALID};
    uint32_t ebo{INVALID};

    // call after setting verts and idxs to upload them to GPU
    void uploadData();

    void bind() const;
    void unbind() const;
    void draw() const;

    static Mesh makeQuad();
    static Mesh makeQuadLines();

  private:
    void createBuffers();
    void allocateBuffers();

    static std::vector<DefaultVertex> quadVertices;
  };
}