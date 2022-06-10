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

if (MSVC)
    add_compile_options(/W4) # /WX if warnings should be treated as errors
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_executable(FirstWindow main.cpp App.cpp Shader.cpp Mesh.cpp)
target_compile_features(FirstWindow PRIVATE cxx_std_20)
target_link_libraries(
	FirstWindow PRIVATE 
  glad_gl_core_46
	glfw
  glm
)