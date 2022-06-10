#include "Mesh.h"

#include <glad/gl.h>

#include <cmath>

namespace ws
{
  Mesh::Mesh(size_t capacity)
      : capacity(capacity)
  {
    verts.reserve(capacity);
    idxs.reserve(capacity);

    createBuffers();
  }

  Mesh::Mesh(const std::vector<DefaultVertex> &vertices, const std::vector<uint32_t> &indices)
      : verts(std::move(vertices)), idxs(std::move(indices))
  {
    size_t cnt = vertices.size();
    // next power of two larger than size
    capacity = static_cast<size_t>(std::pow(2.0, std::ceil(std::log2(cnt))));

    createBuffers();
  }

  void Mesh::createBuffers()
  {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(DefaultVertex) * verts.size(), verts.empty() ? nullptr : verts.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * idxs.size(), idxs.empty() ? nullptr : idxs.data(), GL_STATIC_DRAW);

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
}