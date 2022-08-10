#include <App.h>
#include <Framebuffer.h>
#include <GSAssets.h>
#include <Mesh.h>
#include <Shader.h>
#include <Texture.h>

#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <implot.h>

#include <memory>
#include <random>
#include <string>
#include <unordered_map>

class Boilerplate : public ws::App
{
public:
  std::unordered_map<std::string, std::unique_ptr<ws::Shader>> shaders;
  std::unique_ptr<ws::Mesh> meshQuad;
  std::mt19937 rng;
  std::uniform_real_distribution<float> dist;

  Boilerplate() : App({.name = "MyApp", .width = 1280u, .height = 960u, .shouldDebugOpenGL = true}) {}

  void onInit() final
  {
    rng = std::mt19937{std::random_device{}()};
    dist = std::uniform_real_distribution<float>();

    shaders["quad"] = std::make_unique<ws::Shader>(GS_ASSETS_FOLDER / "shaders/postprocess/main.vert",
                                                   GS_ASSETS_FOLDER / "shaders/postprocess/main.frag");
    meshQuad.reset(new ws::Mesh(ws::Mesh::makeQuad()));
  }

  void onRender([[maybe_unused]] float time, [[maybe_unused]] float deltaTime) final
  {
    const float widthF = static_cast<float>(width);
    const float heightF = static_cast<float>(height);
    const float renderTargetSize[2] = {widthF, heightF};

    ImGui::Begin("DOS Effects");

    static int demoNo{};
    ImGui::Combo("Demo", &demoNo, "Sample Scene into FB\0Snow\0UV Gradient\0");
    if (ImGui::Button("Reload Shaders"))
      for (auto &[name, shader] : shaders)
        shader->reload();
    ImGui::Separator();
    static bool showImGuiDemo = false;
    static bool showImPlotDemo = false;
    ImGui::Checkbox("Show ImGui Demo", &showImGuiDemo);
    ImGui::Checkbox("Show ImPlot Demo", &showImPlotDemo);
    if (showImGuiDemo)
      ImGui::ShowDemoWindow();
    if (showImPlotDemo)
      ImPlot::ShowDemoWindow();
    ImGui::End();

    uint32_t textureId{};
    switch (demoNo)
    {
    case 0:
      textureId = drawASceneIntoAFrameBufferEffect();
      break;
    case 1:
      textureId = snowEffect();
      break;
    case 2:
      textureId = uvGradientEffect();
      break;
    }

    {
      glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      ws::Shader &shader = *shaders["quad"];
      shader.bind();
      shader.setVector2fv("RenderTargetSize", renderTargetSize);
      meshQuad->bind();
      glDisable(GL_DEPTH_TEST);
      glBindTexture(GL_TEXTURE_2D, textureId);
      meshQuad->draw();
    }
  }

  void onDeinit() final
  {
  }

  uint32_t drawASceneIntoAFrameBufferEffect()
  {
    // TODO: make renderTargetSize widely available
    const float widthF = static_cast<float>(width);
    const float heightF = static_cast<float>(height);
    const float renderTargetSize[2] = {widthF, heightF};

    static std::unique_ptr<ws::Framebuffer> framebuffer = std::make_unique<ws::Framebuffer>(width, height);
    static std::unique_ptr<ws::Shader> shaderMain = std::make_unique<ws::Shader>(GS_ASSETS_FOLDER / "shaders/graverlet/main.vert",
                                                                                 GS_ASSETS_FOLDER / "shaders/graverlet/line.frag");
    if (!shaders.contains("main"))
      shaders["main"] = std::move(shaderMain);
    static std::unique_ptr<ws::Mesh> mesh;
    if (!mesh)
      mesh.reset(new ws::Mesh(ws::Mesh::makeQuadLines()));

    framebuffer->bind();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ws::Shader &shader = *shaders["main"];
    shader.bind();
    shader.setVector2fv("RenderTargetSize", renderTargetSize);
    auto proj = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -1.f, 1.f);
    shader.setMatrix4fv("ProjectionFromView", glm::value_ptr(proj));
    mesh->bind();
    mesh->draw();
    framebuffer->unbind();
    return framebuffer->getColorAttachment().getId();
  }

  uint32_t uvGradientEffect()
  {
    static std::unique_ptr<ws::Texture> image = std::make_unique<ws::Texture>(ws::Texture::Specs{width, height, ws::Texture::Format::RGB8, ws::Texture::Filter::Nearest, ws::Texture::Wrap::Repeat});
    const auto &h = image->specs.height;
    const auto &w = image->specs.width;
    static uint8_t *imgData = new uint8_t[3 * h * w];

    for (uint32_t i = 0; i < h; ++i)
    {
      for (uint32_t j = 0; j < w; ++j)
      {
        size_t ix = 3 * (i * w + j);
        imgData[ix + 0] = static_cast<uint8_t>(static_cast<float>(j) / w * 255);
        imgData[ix + 1] = static_cast<uint8_t>(static_cast<float>(i) / h * 255);
        imgData[ix + 2] = 0;
      }
    }
    image->loadPixels(imgData);
    return image->getId();
  }

  uint32_t snowEffect()
  {
    static std::unique_ptr<ws::Texture> image = std::make_unique<ws::Texture>(ws::Texture::Specs{160, 120, ws::Texture::Format::RGB8, ws::Texture::Filter::Nearest, ws::Texture::Wrap::Repeat});
    static uint8_t *imgSnow = new uint8_t[3 * image->specs.height * image->specs.width]{}; // {} initializes with zeroes

    uint32_t snowInitX = static_cast<uint32_t>((image->specs.height - 1) * image->specs.width + dist(rng) * image->specs.width);
    imgSnow[snowInitX * 3] = 255;

    for (uint32_t i = 1; i < image->specs.height; ++i)
    {
      for (uint32_t j = 1; j < image->specs.width - 1; ++j)
      {
        size_t ix = 3 * (i * image->specs.width + j);
        if (imgSnow[ix + 0] == 255)
        {
          size_t ixLeft = 3 * ((i - 1) * image->specs.width + (j - 1));
          size_t ixMid = 3 * ((i - 1) * image->specs.width + (j - 0));
          size_t ixRight = 3 * ((i - 1) * image->specs.width + (j + 1));
          bool leftEmpty = imgSnow[ixLeft] == 0;
          bool midEmpty = imgSnow[ixMid] == 0;
          bool rightEmpty = imgSnow[ixRight] == 0;

          bool removeCurrent = leftEmpty || midEmpty || rightEmpty;

          if (leftEmpty && !midEmpty && !rightEmpty)
            imgSnow[ixLeft + 0] = 255;
          else if (!leftEmpty && midEmpty && !rightEmpty)
            imgSnow[ixMid + 0] = 255;
          else if (!leftEmpty && !midEmpty && rightEmpty)
            imgSnow[ixRight + 0] = 255;
          else if (leftEmpty && midEmpty && !rightEmpty)
          {
            if (dist(rng) < 0.5)
              imgSnow[ixLeft] = 255;
            else
              imgSnow[ixMid] = 255;
          }
          else if (leftEmpty && !midEmpty && rightEmpty)
          {
            if (dist(rng) < 0.5)
              imgSnow[ixLeft] = 255;
            else
              imgSnow[ixRight] = 255;
          }
          else if (!leftEmpty && midEmpty && rightEmpty)
          {
            if (dist(rng) < 0.5)
              imgSnow[ixMid] = 255;
            else
              imgSnow[ixRight] = 255;
          }
          else if (leftEmpty && midEmpty && rightEmpty)
          {
            float rnd = dist(rng);
            if (rnd < 1.0f / 3)
              imgSnow[ixLeft] = 255;
            else if (rnd < 2.0f / 3)
              imgSnow[ixMid] = 255;
            else
              imgSnow[ixRight] = 255;
          }

          if (removeCurrent)
            imgSnow[ix + 0] = 0;
        }
      }
    }
    image->loadPixels(imgSnow);
    return image->getId();
  }
};

int main()
{
  Boilerplate app;
  app.run();
}