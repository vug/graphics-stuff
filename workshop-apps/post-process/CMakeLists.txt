if(MSVC)
  # /WX if warnings should be treated as errors
  add_compile_options(/W4 /external:I${PROJECT_SOURCE_DIR}/dependencies /external:W0)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_executable(PostProcess
  main.cpp)

target_link_libraries(
  PostProcess PRIVATE
  Workshop
)

target_compile_features(PostProcess PRIVATE cxx_std_20)
