#include <App.h>
#include <Shader.h>
#include <Mesh.h>

#include <glad/gl.h>
#include <imgui.h>

#include <memory>
#include <iostream>

class MyApp : public ws::App
{
public:
  std::unique_ptr<ws::Shader> mainShader;
  std::unique_ptr<ws::Mesh> fullscreenQuad;
  const char *mainShaderVertex = R"(
#version 460 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec2 vUV;
layout (location = 3) in vec4 vColor;
layout (location = 4) in vec4 vCustom;

uniform mat4 WorldFromObject;
uniform mat4 ViewFromWorld;
uniform mat4 ProjectionFromView;
uniform vec2 RenderTargetSize;

out VertexData
{
  vec3 position;
  vec3 normal;
  vec2 uv;
  vec4 color;
} vertexData;

void main()
{
  //gl_Position = ProjectionFromView * ViewFromWorld * WorldFromObject * vec4(vPos, 1.0);
  gl_Position = vec4(vPos, 1.0);
  // gl_PointSize = 5.0;

  vertexData.position = vPos;
  vertexData.normal = vNorm;
  vertexData.uv = vUV;
  vertexData.color = vColor;
}
)";

  const char *mainShaderFragment = R"(
#version 460 core

in VertexData
{
  vec3 position;
  vec3 normal;
  vec2 uv;
  vec4 color;
} vertexData;

uniform vec2 RenderTargetSize;

out vec4 FragColor;

void main()
{
  FragColor = vec4(gl_FragCoord.x / RenderTargetSize.x, gl_FragCoord.y / RenderTargetSize.y, 0, 1);
}
)";

  const char *mainShaderFragment2 = R"(
#version 460 core

in VertexData
{
  vec3 position;
  vec3 normal;
  vec2 uv;
  vec4 color;
} vertexData;

uniform vec2 RenderTargetSize;

out vec4 FragColor;

void main()
{
  FragColor = vec4(0, gl_FragCoord.x / RenderTargetSize.x, gl_FragCoord.y / RenderTargetSize.y, 1);
}
)";

public:
  Specs getSpecs() final
  {
    return {.name = "MyApp", .width = 800u, .height = 600u, .shouldDebugOpenGL = true};
  }

  void onInit() final
  {
    mainShader = std::make_unique<ws::Shader>(mainShaderVertex, mainShaderFragment);
    fullscreenQuad.reset(new ws::Mesh(ws::Mesh::makeQuad())); // does not call Mesh destructor

    glEnable(GL_DEPTH_TEST);
  }

  void onRender([[maybe_unused]] float time, [[maybe_unused]] float deltaTime) final
  {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float rts[2] = {static_cast<float>(getSpecs().width), static_cast<float>(getSpecs().height)};

    mainShader->bind();
    mainShader->setVector2fv("RenderTargetSize", rts);
    fullscreenQuad->uploadData();
    glBindVertexArray(fullscreenQuad->vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(fullscreenQuad->idxs.size()), GL_UNSIGNED_INT, 0);

    ImGui::Begin("Shader Study");
    if (ImGui::Button("Recompile - Fragment1"))
    {
      mainShader->compile(mainShaderVertex, mainShaderFragment);
      printf("shader program %d shaders: ", mainShader->getId());
      for (const auto &id : mainShader->getShaderIds())
        printf("%d ", id);
      printf("\n");
    }
    if (ImGui::Button("Recompile - Fragment2"))
    {
      mainShader->compile(mainShaderVertex, mainShaderFragment2);
      printf("shader program %d shaders: ", mainShader->getId());
      for (const auto &id : mainShader->getShaderIds())
        printf("%d ", id);
      printf("\n");
    }
    ImGui::End();
  }

  void onDeinit() final
  {
  }
};

int main()
{
  MyApp app;
  app.run();
}