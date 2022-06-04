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
static inline uint8_t ImSaturate(uint8_t f) { return (f < 0) ? 0 : (f > 255) ? 255
                                                                             : f; }

int width = 800;
int height = 600;

class MyImage
{
public:
  MyImage(int w, int h, const std::vector<glm::u8vec4> &pixels)
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

  MyImage(int w, int h)
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

  GLFWwindow *window = glfwCreateWindow(width, height, "Skeleton", nullptr, nullptr);
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
  float clearColor[3] = {0.2f, 0.3f, 0.3f};

  glm::u8vec3 colorPicker = {51, 77, 77};
  const int barWidth = 32, barHeight = 256;
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
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImGui::Checkbox("Demo", &showDemo);
    {
      std::vector<glm::u8vec4> pixels1(barWidth * barHeight);
      std::vector<glm::u8vec4> pixels2(barWidth * barHeight);
      std::vector<glm::u8vec4> pixels3(barWidth * barHeight);
      for (uint32_t i = 0; i < barHeight; ++i)
        for (uint32_t j = 0; j < barWidth; ++j)
        {
          const int ix = (i * barWidth + j);
          pixels1[ix] = {static_cast<uint8_t>(i), colorPicker.g, colorPicker.b, 255};
          pixels2[ix] = {colorPicker.r, static_cast<uint8_t>(i), colorPicker.b, 255};
          pixels3[ix] = {colorPicker.r, colorPicker.g, static_cast<uint8_t>(i), 255};
        }
      bar1.updateData(pixels1);
      bar2.updateData(pixels2);
      bar3.updateData(pixels3);
    }
    int col[3] = {colorPicker.r, colorPicker.g, colorPicker.b};
    if (ImGui::DragInt3("RGB", col, 1, 0, 255, "%3d", ImGuiSliderFlags_None))
    {
      colorPicker.r = static_cast<uint8_t>(col[0]);
      colorPicker.g = static_cast<uint8_t>(col[1]);
      colorPicker.b = static_cast<uint8_t>(col[2]);
    }
    ImVec2 m = ImGui::GetMousePos();
    // TODO: abstract bar. when changed, updates the arrow, returns/sets selected value in [0.f, 1.f] range.
    // then, set colorPicker color value based on the colorspace choice.
    ImVec2 bar1Pos = ImGui::GetCursorScreenPos();
    ImGui::Image((void *)(intptr_t)bar1.getId(), {static_cast<float>(bar1.getWidth()), static_cast<float>(bar1.getHeight())});
    ImGui::SetCursorScreenPos(bar1Pos);
    ImGui::InvisibleButton("bar1Button", {32, 256});
    // Note that it is active as long as mouse button is pressed
    if (ImGui::IsItemActive())
      colorPicker.r = ImSaturate(static_cast<uint8_t>(m.y - bar1Pos.y));
    {
      float x = bar1Pos.x;
      float y = bar1Pos.y + colorPicker.r / 255.0 * barHeight;
      draw_list->AddTriangle({x, y}, {x - 3, y - 3}, {x - 3, y + 3}, 0xFFFFFFFF, 1.0f);
    }
    ImGui::SameLine();
    ImVec2 bar2Pos = ImGui::GetCursorScreenPos();
    ImGui::Image((void *)(intptr_t)bar2.getId(), {static_cast<float>(bar2.getWidth()), static_cast<float>(bar2.getHeight())});
    ImGui::SetCursorScreenPos(bar2Pos);
    ImGui::InvisibleButton("bar2Button", {32, 256});
    if (ImGui::IsItemActive())
      colorPicker.g = ImSaturate(static_cast<uint8_t>(m.y - bar2Pos.y));
    ImGui::SameLine();
    ImVec2 bar3Pos = ImGui::GetCursorScreenPos();
    ImGui::Image((void *)(intptr_t)bar3.getId(), {static_cast<float>(bar1.getWidth()), static_cast<float>(bar1.getHeight())});
    ImGui::SetCursorScreenPos(bar3Pos);
    ImGui::InvisibleButton("bar3Button", {32, 256});
    if (ImGui::IsItemActive())
      colorPicker.b = ImSaturate(static_cast<uint8_t>(m.y - bar3Pos.y));
    ImVec4 imCol = {static_cast<float>(colorPicker.r) / 255, static_cast<float>(colorPicker.g) / 255, static_cast<float>(colorPicker.b) / 255, 1.0};
    ImGui::ColorButton("picked color", imCol, ImGuiColorEditFlags_None, {50, 50});
    ImGui::Text("bar1: (%.0f, %.0f), bar2: (%.0f, %.0f), bar3: (%.0f, %.0f)", bar1Pos.x, bar1Pos.y, bar2Pos.x, bar2Pos.y, bar3Pos.x, bar3Pos.y);
    ImGui::Separator();
    ImGui::ColorPicker3("clear color", clearColor, ImGuiColorEditFlags_None);
    ImGui::End();

    ImGui::Render();

    glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
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
  width = w;
  height = h;
  glViewport(0, 0, width, height);
}