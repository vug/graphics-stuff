if(MSVC)
  # /WX if warnings should be treated as errors
  add_compile_options(/W4 /external:I${PROJECT_SOURCE_DIR}/dependencies /external:W0)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_library(Workshop STATIC
  App.cpp
  Shader.cpp
  Texture.cpp Framebuffer.cpp
  Mesh.cpp OMesh.cpp
  Camera.cpp CameraController.cpp)

target_compile_features(Workshop PRIVATE cxx_std_20)

target_link_libraries(
  Workshop PUBLIC
  glad_gl_core_46
  glfw
  glm
  imgui
  implot
  OpenMeshCore
  stb
)

target_compile_options(
  Workshop PUBLIC
  -D_USE_MATH_DEFINES
)

target_include_directories(Workshop PUBLIC .)