cmake_minimum_required(VERSION 3.20)

project(GraphicsEngineersWorkshop
        VERSION 0.1
        DESCRIPTION "Graphics Engineer's Workshop")

add_subdirectory(../dependencies/glfw glfw)

add_executable(FirstWindow main.cpp)
target_compile_features(FirstWindow PRIVATE cxx_std_20)
target_link_libraries(
	FirstWindow PRIVATE 
	glfw
  opengl
)