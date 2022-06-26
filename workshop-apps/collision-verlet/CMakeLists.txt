if(MSVC)
  add_compile_options(/W4) # /WX if warnings should be treated as errors
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_executable(CollisionVerlet
  main.cpp)

target_link_libraries(
  CollisionVerlet PRIVATE
  Workshop
)

target_compile_features(CollisionVerlet PRIVATE cxx_std_20)
