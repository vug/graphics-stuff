add_executable(FirstWindow
  cellular/main.cpp cellular/cell.cpp
  App.cpp Shader.cpp Mesh.cpp OMesh.cpp Camera.cpp)
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

if(MSVC)
  add_compile_options(/W4) # /WX if warnings should be treated as errors
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()