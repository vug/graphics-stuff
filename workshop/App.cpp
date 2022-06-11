#include "App.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>

namespace ws
{
  static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
  static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

  static inline const char *glMessageSourceToString(GLenum source);
  static inline const char *glMessageTypeToString(GLenum type);
  static inline const char *glMessageSeverityToString(GLenum severity);
  void GLAPIENTRY OpenGLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                             GLsizei length, const char *message, const void *userParam);

  void App::run()
  {
    // Tried providing specs in App constructor. But user has to write Derived constructor with matching input params
    // Tried settings specs in App constructor list via this method, but cannot use virtual methods in ctor, dtor
    // Therefore putting it to this weird place. In Walnut, App is not derived, but Layers with virtual methods are given
    specs = getSpecs();

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);

    GLFWwindow *window = glfwCreateWindow(specs.width, specs.height, specs.name.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(window);
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

    glViewport(0, 0, specs.width, specs.height);
    if (specs.shouldDebugOpenGL)
    {
      glEnable(GL_MULTISAMPLE);
      glEnable(GL_DEBUG_OUTPUT);
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      glDebugMessageCallback(OpenGLDebugMessageCallback, nullptr);
      // Ignore notifications
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
    }

    onInit();

    while (!glfwWindowShouldClose(window))
    {
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      glfwPollEvents();

      onRender();

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
      return "Unknown";
    }
  }

  void GLAPIENTRY OpenGLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                             GLsizei length, const char *message, const void *userParam)
  {
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
      std::cout << "OpenGL Debug Message [" << id << "]. Severity: " << glMessageSeverityToString(severity) << ". Source: " << glMessageSourceToString(source) << ". Type: " << glMessageTypeToString(type) << ". Message: " << message << ".\n";
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      std::cout << "OpenGL Debug Message [" << id << "]. Severity: " << glMessageSeverityToString(severity) << ". Source: " << glMessageSourceToString(source) << ". Type: " << glMessageTypeToString(type) << ". Message: " << message << ".\n";
      break;
    case GL_DEBUG_SEVERITY_LOW:
      std::cout << "OpenGL Debug Message [" << id << "]. Severity: " << glMessageSeverityToString(severity) << ". Source: " << glMessageSourceToString(source) << ". Type: " << glMessageTypeToString(type) << ". Message: " << message << ".\n";
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      std::cout << "OpenGL Debug Message [" << id << "]. Severity: " << glMessageSeverityToString(severity) << ". Source: " << glMessageSourceToString(source) << ". Type: " << glMessageTypeToString(type) << ". Message: " << message << ".\n";
      break;
    default:
      assert(false); // unknown severity
    }
  }
}