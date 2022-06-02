/**
 * bring OpenGL functions via Glad. Set clear color.
 * 
 * modified from: http://learnopengl.com/code_viewer.php?code=getting-started/hellowindow2 GLEW replaced with Glad
 */
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

int main()
{
    const int width = 800;
    const int height = 600;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(width, height, "OpenGL w/GLFW + Glad", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    gladLoadGL();

    glViewport(0, 0, width, height);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}