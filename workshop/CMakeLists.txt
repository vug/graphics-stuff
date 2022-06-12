cmake_minimum_required(VERSION 3.20)

project(GraphicsEngineersWorkshop
        VERSION 0.1
        DESCRIPTION "Graphics Engineer's Workshop")

# https://github.com/glfw/glfw
add_subdirectory(../dependencies/glfw glfw_cmake)

# https://github.com/Dav1dde/glad/ glad2 branch
# https://github.com/Dav1dde/glad/blob/glad2/cmake/CMakeLists.txt
add_subdirectory(../dependencies/glad/cmake glad_cmake)
glad_add_library(glad_gl_core_46 STATIC API gl:core=4.6)

# https://github.com/g-truc/glm
add_subdirectory(../dependencies/glm glm_cmake)

# https://github.com/ocornut/imgui
add_library(
  imgui
  ../dependencies/imgui/imgui.cpp
  ../dependencies/imgui/imgui_draw.cpp
  ../dependencies/imgui/imgui_widgets.cpp
  ../dependencies/imgui/imgui_tables.cpp
  ../dependencies/imgui/imgui_demo.cpp # for ImGui::ShowDemoWindow()
  ../dependencies/imgui/backends/imgui_impl_glfw.cpp
  ../dependencies/imgui/backends/imgui_impl_opengl3.cpp
  ../dependencies/imgui/misc/cpp/imgui_stdlib.cpp
)
target_compile_options(
  imgui PRIVATE
  -DIMGUI_IMPL_OPENGL_LOADER_GLAD # configure GLAD as the loader
)
target_link_libraries(
  imgui PRIVATE 
  glfw
  )
  target_include_directories(
    imgui PUBLIC
    ../dependencies/imgui/
    ../dependencies/imgui//backends
    ../dependencies/imgui//misc/cpp
    )
    
# https://github.com/Lawrencemm/openmesh
add_subdirectory(../dependencies/openmesh openmesh_cmake)
set(BUILD_APPS OFF) # we don't need the QT apps that comes with OpenMesh
    
add_executable(FirstWindow main.cpp App.cpp Shader.cpp Mesh.cpp OMesh.cpp Camera.cpp)
target_compile_features(FirstWindow PRIVATE cxx_std_20)
target_link_libraries(
	FirstWindow PRIVATE 
  glad_gl_core_46
	glfw
  glm
  imgui
  OpenMeshCore
)
target_compile_options(
  FirstWindow PRIVATE
  -D_USE_MATH_DEFINES
)

if (MSVC)
    add_compile_options(/W4) # /WX if warnings should be treated as errors
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()