#include "Mesh.h"

#include <glad/gl.h>

#include <cmath>

namespace ws
{
  Mesh::Mesh(size_t capacity)
      : capacity(capacity)
  {
    verts.resize(capacity);
    idxs.resize(capacity);

    createBuffers();
    uploadData();
  }

  Mesh::Mesh(const std::vector<DefaultVertex> &vertices, const std::vector<uint32_t> &indices)
      : verts(std::move(vertices)), idxs(std::move(indices))
  {
    size_t cnt = vertices.size();
    // next power of two larger than size
    capacity = static_cast<size_t>(std::pow(2.0, std::ceil(std::log2(cnt))));

    createBuffers();
    uploadData();
  }

  Mesh::~Mesh()
  {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
  }

  void Mesh::createBuffers()
  {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    allocateBuffers();

    static const std::vector<int32_t> sizes = {3, 3, 2, 4, 4, 4};
    size_t offset = 0;
    for (int ix = 0; ix < sizes.size(); ++ix)
    {
      glVertexAttribPointer(ix, sizes[ix], GL_FLOAT, GL_FALSE, sizeof(DefaultVertex), (void *)offset);
      glEnableVertexAttribArray(ix);
      offset += sizes[ix] * sizeof(float);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  void Mesh::allocateBuffers()
  {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(DefaultVertex) * capacity, nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * capacity, nullptr, GL_DYNAMIC_DRAW);
  }

  void Mesh::uploadData()
  {
    glBindVertexArray(vao);
    if (capacity < idxs.size())
    {
      do
      {
        capacity *= 2;
      } while (capacity <= idxs.size());

      allocateBuffers();
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(DefaultVertex) * verts.size(), verts.data());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * idxs.size(), idxs.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}