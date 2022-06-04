#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

void glfw_error_callback(int error, const char *description);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

int width = 800;
int height = 600;

class MyImage
{
public:
  MyImage(int w, int h, std::vector<uint8_t> img_data)
      : width(w), height(h)
  {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data.data());
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  MyImage(int w, int h)
      : MyImage(w, h, std::vector<uint8_t>(w * h * 4)) {}

  void updateData(std::vector<uint8_t> img_data)
  {
    glBindTexture(GL_TEXTURE_2D, id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, img_data.data());
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

  MyImage img{256, 256};
  {
    std::vector<uint8_t> data(img.getWidth() * img.getHeight() * 4);
    for (uint32_t i = 0; i < img.getHeight(); ++i)
      for (uint32_t j = 0; j < img.getWidth(); ++j)
      {
        const int ix = (i * img.getWidth() + j) * 4;
        data[ix + 0] = static_cast<unsigned char>(i);
        data[ix + 1] = static_cast<unsigned char>(0);
        data[ix + 2] = static_cast<unsigned char>(j);
        data[ix + 3] = static_cast<unsigned char>(255);
      }
    img.updateData(data);
  }

  while (!glfwWindowShouldClose(window))
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (showDemo)
      ImGui::ShowDemoWindow();

    ImGui::Begin("Main");
    ImGui::Checkbox("Demo", &showDemo);
    ImGui::Image((void *)(intptr_t)img.getId(), {static_cast<float>(img.getWidth()), static_cast<float>(img.getHeight())});
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