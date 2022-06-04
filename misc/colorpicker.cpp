#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <vector>

void glfw_error_callback(int error, const char *description);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

bool ImBar(const char *str_id, float &val, uint32_t textureId, ImVec2 size);

int appWidth = 800;
int appHeight = 600;

class MyImage
{
public:
  MyImage(uint32_t w, uint32_t h, const std::vector<glm::u8vec4> &pixels)
      : width(w), height(h)
  {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  MyImage(uint32_t w, uint32_t h)
      : MyImage(w, h, std::vector<glm::u8vec4>(w * h * 4)) {}

  void updateData(const std::vector<glm::u8vec4> &pixels)
  {
    glBindTexture(GL_TEXTURE_2D, id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  uint32_t getWidth() const { return width; }
  uint32_t getHeight() const { return height; }
  uint32_t getId() const { return id; }

private:
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t id = 0;
};

int main()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 8);

  GLFWwindow *window = glfwCreateWindow(appWidth, appHeight, "Skeleton", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetKeyCallback(window, key_callback);
  glfwSetErrorCallback(glfw_error_callback);

  gladLoadGL();

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows

  ImGui::StyleColorsDark();
  // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
  ImGuiStyle &style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 460");

  bool showDemo = false;

  float colorRGB[3] = {51.f / 255.f, 77.f / 255.f, 77.f / 255.f}; // source of truth
  const ImVec2 barSize = {64.f, 256.f};
  const uint32_t barWidth = static_cast<uint32_t>(barSize.x);
  const uint32_t barHeight = static_cast<uint32_t>(barSize.y);
  MyImage bar1{barWidth, barHeight};
  MyImage bar2{barWidth, barHeight};
  MyImage bar3{barWidth, barHeight};

  while (!glfwWindowShouldClose(window))
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (showDemo)
      ImGui::ShowDemoWindow();

    ImGui::Begin("Main");
    ImGui::Checkbox("Demo", &showDemo);
    ImGui::Separator();

    glm::u8vec3 colorRGBBytes = {static_cast<uint8_t>(colorRGB[0] * 255.f), static_cast<uint8_t>(colorRGB[1] * 255.f), static_cast<uint8_t>(colorRGB[2] * 255.f)};
    {
      std::vector<glm::u8vec4> pixels1(barWidth * barHeight);
      std::vector<glm::u8vec4> pixels2(barWidth * barHeight);
      std::vector<glm::u8vec4> pixels3(barWidth * barHeight);
      for (uint32_t i = 0; i < barHeight; ++i)
        for (uint32_t j = 0; j < barWidth; ++j)
        {
          const int ix = (i * barWidth + j);
          pixels1[ix] = {static_cast<uint8_t>(i), colorRGBBytes.g, colorRGBBytes.b, 255};
          pixels2[ix] = {colorRGBBytes.r, static_cast<uint8_t>(i), colorRGBBytes.b, 255};
          pixels3[ix] = {colorRGBBytes.r, colorRGBBytes.g, static_cast<uint8_t>(i), 255};
        }
      bar1.updateData(pixels1);
      bar2.updateData(pixels2);
      bar3.updateData(pixels3);
    }

    ImGui::DragFloat3("RGB", colorRGB, 1.f / 255.f, 0.f, 1.f, "%.3f", ImGuiSliderFlags_None);

    float colorHSV[3];
    ImGui::ColorConvertRGBtoHSV(colorRGB[0], colorRGB[1], colorRGB[2], colorHSV[0], colorHSV[1], colorHSV[2]);
    if (ImGui::DragFloat3("HSV", colorHSV, 1.f / 255.f, 0.0f, 1.0f, "%.3f"))
      ImGui::ColorConvertHSVtoRGB(colorHSV[0], colorHSV[1], colorHSV[2], colorRGB[0], colorRGB[1], colorRGB[2]);

    ImBar("bar1", colorRGB[0], bar1.getId(), barSize);
    ImGui::SameLine();
    ImBar("bar2", colorRGB[1], bar2.getId(), barSize);
    ImGui::SameLine();
    ImBar("bar3", colorRGB[2], bar3.getId(), barSize);
    ImGui::SameLine();

    ImVec4 imCol = {colorRGB[0], colorRGB[1], colorRGB[2], 1.0};
    ImGui::ColorButton("picked color", imCol, ImGuiColorEditFlags_None, {64, 64});
    ImGui::Separator();

    // TODO: remove when done with my color picker
    static float someColor[3] = {0.2f, 0.3f, 0.3f};
    ImGui::ColorPicker3("clear color", someColor, ImGuiColorEditFlags_None);
    ImGui::End();

    ImGui::Render();

    glClearColor(colorRGB[0], colorRGB[1], colorRGB[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      GLFWwindow *backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
  return 0;
}

void glfw_error_callback(int error, const char *description)
{
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void key_callback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mode)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, int w, int h)
{
  appWidth = w;
  appHeight = h;
  glViewport(0, 0, appWidth, appHeight);
}

bool ImBar(const char *str_id, float &val, uint32_t textureId, ImVec2 size)
{
  bool hasChanged = false;
  const ImVec2 barPos = ImGui::GetCursorScreenPos();
  ImGui::Image((void *)(intptr_t)textureId, size);
  ImGui::SetCursorScreenPos(barPos);
  ImGui::InvisibleButton(str_id, size);
  // Active as long as mouse button is pressed
  if (ImGui::IsItemActive())
  {
    const ImVec2 m = ImGui::GetMousePos();
    val = (m.y - barPos.y) / size.y;
    val = std::min(std::max(0.f, val), 1.f);
    hasChanged = true;
  }
  ImDrawList *draw_list = ImGui::GetWindowDrawList();
  float x = barPos.x;
  float y = barPos.y + val * size.y;
  draw_list->AddTriangle({x, y}, {x - 3, y - 3}, {x - 3, y + 3}, 0xFFFFFFFF, 1.0f);
  return hasChanged;
}