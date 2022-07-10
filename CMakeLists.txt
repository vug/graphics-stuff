cmake_minimum_required(VERSION 3.20)

project(GraphicsEngineersWorkshop
  VERSION 0.1
  DESCRIPTION "Graphics Engineer's Workshop")

add_subdirectory(dependencies)
add_subdirectory(workshop)
add_subdirectory(workshop-apps/boilerplate)
add_subdirectory(workshop-apps/cellular)
add_subdirectory(workshop-apps/collision-verlet)
add_subdirectory(workshop-apps/shader-study)
add_subdirectory(workshop-apps/graverlet)