cmake_minimum_required(VERSION 3.20)

project(GraphicsEngineersWorkshop
  VERSION 0.1
  DESCRIPTION "Graphics Engineer's Workshop")

set(GS_ASSETS_FOLDER ${PROJECT_SOURCE_DIR}/assets)
configure_file(assets/GSAssets.h.in assets/GSAssets.h @ONLY)
include_directories(${PROJECT_BINARY_DIR}/assets)

add_subdirectory(dependencies)
add_subdirectory(workshop)
add_subdirectory(workshop-apps/boilerplate)

# add_subdirectory(workshop-apps/cellular)
# add_subdirectory(workshop-apps/collision-verlet)
add_subdirectory(workshop-apps/dos-effects)
add_subdirectory(workshop-apps/compute-shader-study)

# add_subdirectory(workshop-apps/graverlet)
# add_subdirectory(workshop-apps/post-process)
# add_subdirectory(workshop-apps/shader-study)