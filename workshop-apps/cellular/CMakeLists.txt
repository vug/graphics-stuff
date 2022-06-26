add_executable(Cellular
  main.cpp cell.cpp)

target_link_libraries(
  Cellular PRIVATE
  Workshop
)

target_compile_features(Cellular PRIVATE cxx_std_20)

if(MSVC)
  add_compile_options(/W4) # /WX if warnings should be treated as errors
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()