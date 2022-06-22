# https://github.com/glfw/glfw
add_subdirectory(glfw)

# https://github.com/Dav1dde/glad/ glad2 branch
# https://github.com/Dav1dde/glad/blob/glad2/cmake/CMakeLists.txt
add_subdirectory(glad/cmake)
glad_add_library(glad_gl_core_46 STATIC API gl:core=4.6)

# https://github.com/g-truc/glm
add_subdirectory(glm)

# https://github.com/ocornut/imgui
add_library(
  imgui
  imgui/imgui.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_widgets.cpp
  imgui/imgui_tables.cpp
  imgui/imgui_demo.cpp # for ImGui::ShowDemoWindow()
  imgui/backends/imgui_impl_glfw.cpp
  imgui/backends/imgui_impl_opengl3.cpp
  imgui/misc/cpp/imgui_stdlib.cpp
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
  imgui/
  imgui/backends
  imgui/misc/cpp
)

# https://github.com/Lawrencemm/openmesh
# INTERNAL hides the value. Could have been BOOL, then parameter appears in "cmake .. -L"
set(BUILD_APPS false CACHE INTERNAL "" FORCE) # we don't need the QT apps that comes with OpenMesh
set(OPENMESH_DOCS false CACHE INTERNAL "" FORCE)
add_subdirectory(openmesh)