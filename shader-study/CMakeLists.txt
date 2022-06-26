if(MSVC)
  add_compile_options(/W4) # /WX if warnings should be treated as errors
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

set(SHADER_STUDY_ASSETS_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(ShaderStudyAssets.h.in ShaderStudyAssets.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(ShaderStudy
  main.cpp)

target_link_libraries(
  ShaderStudy PRIVATE
  Workshop
)

target_compile_features(ShaderStudy PRIVATE cxx_std_20)
