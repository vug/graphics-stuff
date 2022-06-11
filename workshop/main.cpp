/**
 * bring OpenGL functions via Glad. Set clear color.
 */
#include "App.h"
#include "Shader.h"
#include "Mesh.h"

#include <glad/gl.h>

#include <string>
#include <memory>

class MyApp : public ws::App
{
public:
  const char *vertexShaderSource = R"(
  #version 460 core
  layout (location = 0) in vec3 aPos;
  void main()
  {
      gl_Position = vec4(aPos, 1.0);
  }
  )";

  const char *fragmentShaderSource = R"(
  #version 460 core
  out vec4 FragColor;
  void main()
  {
      FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
  }
  )";

  // Because these don't have default constructors, can't make them members in class scope
  std::unique_ptr<ws::Shader> mainShader;
  std::unique_ptr<ws::Mesh> mesh;

  Specs getSpecs() final
  {
    return {"MyApp", 800, 600};
  }

  void onInit() final
  {
    mainShader = std::make_unique<ws::Shader>(vertexShaderSource, fragmentShaderSource);
    // ws::Mesh mesh{128}; // TODO: try out this one.
    std::vector<glm::vec3> positions = {
        {0.0f, 0.0f, 0.0f}, // p1
        {0.0f, 0.1f, 0.0f}, // p2
        {0.1f, 0.0f, 0.0f}, // p3
    };
    std::vector<uint32_t> indices = {
        0, 1, 2, // triangle1
    };
    std::vector<ws::DefaultVertex> vertices;
    for (const auto &p : positions)
    {
      ws::DefaultVertex v{.position = p};
      vertices.push_back(v);
    }
    mesh = std::make_unique<ws::Mesh>(vertices, indices);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  void onRender() final
  {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(mainShader->id);
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->idxs.size()), GL_UNSIGNED_INT, 0);
  }

  void onDeinit() final {}
};

int main()
{
  MyApp app;
  app.run();
  return 0;
}
