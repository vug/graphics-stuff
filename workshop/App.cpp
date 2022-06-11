#include "App.h"

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>

namespace ws
{
  static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
  static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

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

    GLFWwindow *window = glfwCreateWindow(specs.width, specs.height, specs.name.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    gladLoadGL(glfwGetProcAddress);

    glViewport(0, 0, specs.width, specs.height);

    onInit();

    while (!glfwWindowShouldClose(window))
    {
      glfwPollEvents();

      onRender();

      glfwSwapBuffers(window);
    }

    onDeinit();
  }

  App::~App()
  {
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
}