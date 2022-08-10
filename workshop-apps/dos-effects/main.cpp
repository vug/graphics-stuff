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
  std::unique_ptr<ws::Mesh> mesh;
  std::unique_ptr<ws::Mesh> meshQuad;
  std::unique_ptr<ws::Framebuffer> framebuffer;
  std::unique_ptr<ws::Framebuffer> framebuffer2;
  std::unique_ptr<ws::Texture> image;
  std::mt19937 rng;
  std::uniform_real_distribution<float> dist;
  uint8_t *imgSnow = new uint8_t[3 * height * width];

  Boilerplate() : App({.name = "MyApp", .width = 1280u, .height = 960u, .shouldDebugOpenGL = true}) {}

  void onInit() final
  {
    rng = std::mt19937{std::random_device{}()};
    dist = std::uniform_real_distribution<float>();

    shaders["main"] = std::make_unique<ws::Shader>(GS_ASSETS_FOLDER / "shaders/graverlet/main.vert",
                                                   GS_ASSETS_FOLDER / "shaders/graverlet/line.frag");
    shaders["quad"] = std::make_unique<ws::Shader>(GS_ASSETS_FOLDER / "shaders/postprocess/main.vert",
                                                   GS_ASSETS_FOLDER / "shaders/postprocess/main.frag");

    mesh.reset(new ws::Mesh(ws::Mesh::makeQuadLines()));

    meshQuad.reset(new ws::Mesh(ws::Mesh::makeQuad()));

    framebuffer = std::make_unique<ws::Framebuffer>(width, height);

    image = std::make_unique<ws::Texture>(ws::Texture::Specs{width, height, ws::Texture::Format::RGB8, ws::Texture::Filter::Nearest, ws::Texture::Wrap::Repeat});
    image = std::make_unique<ws::Texture>(ws::Texture::Specs{160, 120, ws::Texture::Format::RGB8, ws::Texture::Filter::Nearest, ws::Texture::Wrap::Repeat});

    for (uint32_t i = 0; i < height; ++i)
    {
      for (uint32_t j = 0; j < width; ++j)
      {
        size_t ix = 3 * (i * width + j);
        imgSnow[ix + 0] = 0;
        imgSnow[ix + 1] = 0;
        imgSnow[ix + 2] = 0;
      }
    }
  }

  void onRender([[maybe_unused]] float time, [[maybe_unused]] float deltaTime) final
  {
    const float widthF = static_cast<float>(width);
    const float heightF = static_cast<float>(height);
    const float renderTargetSize[2] = {widthF, heightF};

    ImGui::Begin("Boilerplate");
    static bool showImGuiDemo = false;
    static bool showImPlotDemo = false;
    ImGui::Checkbox("Show ImGui Demo", &showImGuiDemo);
    ImGui::Checkbox("Show ImPlot Demo", &showImPlotDemo);
    if (showImGuiDemo)
      ImGui::ShowDemoWindow();
    if (showImPlotDemo)
      ImPlot::ShowDemoWindow();
    ImGui::Separator();
    if (ImGui::Button("Reload"))
      for (auto &[name, shader] : shaders)
        shader->reload();
    ImGui::End();

    {
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
    }

    static uint32_t frameNo = 0;
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

    // static uint8_t *imgData = new uint8_t[3 * height * width];
    // for (uint32_t i = 0; i < height; ++i)
    // {
    //   for (uint32_t j = 0; j < width; ++j)
    //   {
    //     size_t ix = 3 * (i * width + j);
    //     imgData[ix + 0] = static_cast<uint8_t>(static_cast<float>(j) / width * 255);
    //     imgData[ix + 1] = static_cast<uint8_t>(static_cast<float>(i) / height * 255);
    //     imgData[ix + 2] = 0;
    //   }
    // }
    // image->loadPixels(imgData);

    {
      glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      ws::Shader &shader = *shaders["quad"];
      shader.bind();
      shader.setVector2fv("RenderTargetSize", renderTargetSize);
      meshQuad->bind();
      glDisable(GL_DEPTH_TEST);
      // glBindTexture(GL_TEXTURE_2D, framebuffer->getColorAttachment().getId());
      glBindTexture(GL_TEXTURE_2D, image->getId());
      meshQuad->draw();
    }

    ++frameNo;
  }

  void onDeinit() final
  {
  }
};

int main()
{
  Boilerplate app;
  app.run();
}