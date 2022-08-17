#include "App.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <cassert>

namespace ws
{
  static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
  static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

  static inline const char *glMessageSourceToString(GLenum source);
  static inline const char *glMessageTypeToString(GLenum type);
  static inline const char *glMessageSeverityToString(GLenum severity);
  void GLAPIENTRY OpenGLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                             GLsizei length, const char *message, const void *userParam);

  App::App(const Specs &specs) : specs(specs)
  {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    if (specs.shouldDebugOpenGL)
      glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_SAMPLES, 8);

    width = specs.width;
    height = specs.height;
    GLFWwindow *window = glfwCreateWindow(width, height, specs.name.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    gladLoadGL(glfwGetProcAddress);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
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
    ImPlot::CreateContext();

    glViewport(0, 0, width, height);
    glEnable(GL_MULTISAMPLE);
    if (specs.shouldDebugOpenGL)
    {
      glEnable(GL_DEBUG_OUTPUT);
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      glDebugMessageCallback(OpenGLDebugMessageCallback, this);
      // Ignore notifications
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
    }
  }

  void App::run()
  {
    onInit();

    float time = static_cast<float>(glfwGetTime());
    ImGuiIO &io = ImGui::GetIO();
    GLFWwindow *window = glfwGetCurrentContext();

    while (!glfwWindowShouldClose(window))
    {
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      glfwPollEvents();
      glfwGetWindowPos(window, &winPosX, &winPosY);

      const float deltaTime = static_cast<float>(glfwGetTime()) - time;
      time += deltaTime;
      onRender(time, deltaTime);

      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
      if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
      }

      glfwSwapBuffers(window);
    }

    onDeinit();
  }

  App::~App()
  {
    ImPlot::DestroyContext();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
  }

  void key_callback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mode)
  {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
  }

  void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, int width, int height)
  {
    glViewport(0, 0, width, height);
    void *ptr = glfwGetWindowUserPointer(window);
    if (ptr)
    {
      App *app = static_cast<App *>(ptr);
      app->width = width;
      app->height = height;
    }
  }

  inline const char *glMessageSourceToString(GLenum source)
  {
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
      return "OpenGL API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      return "Window System";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      return "Shader compiler";
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      return "Third-party app associated with OpenGL";
    case GL_DEBUG_SOURCE_APPLICATION:
      return "The user of this application";
    case GL_DEBUG_SOURCE_OTHER:
      return "Unspecified";
    default:
      assert(false); // unknown source
      return "Unknown";
    }
  };

  inline const char *glMessageTypeToString(GLenum type)
  {
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
      return "Error";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      return "Deprecated behavior";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      return "Undefined behavior";
    case GL_DEBUG_TYPE_PORTABILITY:
      return "Unportable functionality";
    case GL_DEBUG_TYPE_PERFORMANCE:
      return "Performance issue";
    case GL_DEBUG_TYPE_MARKER:
      return "Command stream annotation";
    case GL_DEBUG_TYPE_PUSH_GROUP:
      return "Group pushing";
    case GL_DEBUG_TYPE_POP_GROUP:
      return "Group popping";
    case GL_DEBUG_TYPE_OTHER:
      return "Unspecified";
    default:
      assert(false); // unknown type
      return "Unknown";
    }
  }

  inline const char *glMessageSeverityToString(GLenum severity)
  {
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
      return "High";
    case GL_DEBUG_SEVERITY_MEDIUM:
      return "Medium";
    case GL_DEBUG_SEVERITY_LOW:
      return "Low";
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      return "Notification";
    default:
      assert(false); // unknown type
      return "Unknown";
    }
  }

  void GLAPIENTRY OpenGLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                             [[maybe_unused]] GLsizei length, const char *message, const void *userParam)
  {
    // filter out non-significant error/warning codes
    if (
        false
        // id == 131169    //
        // || id == 131185 // Buffer object 2 (bound to GL_ELEMENT_ARRAY_BUFFER_ARB, usage hint is GL_STATIC_DRAW) will use VIDEO memory as the source for buffer object operations
        // || id == 131218 // Program/shader state performance warning: Vertex shader in program 9 is being recompiled based on GL state.
        // || id == 131204 //
        // || id == 2      // INFO: API_ID_RECOMPILE_FRAGMENT_SHADER performance warning has been generated. Fragment shader recompiled due to state change. [ID: 2]
    )
      return;

    std::cout << "OpenGL Debug Message [" << id << "]"
              << ". Severity : " << glMessageSeverityToString(severity)
              << ". Source: " << glMessageSourceToString(source)
              << ". Type: " << glMessageTypeToString(type)
              << ". Message: " << message << ".\n";

    App *thisApp = (App *)userParam;
    if (thisApp->specs.shouldBreakAtOpenGLDebugCallback)
      __debugbreak();
  }
}