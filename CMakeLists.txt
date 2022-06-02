cmake_minimum_required(VERSION 3.12)

set(PROJECT_NAME Erosion3D)
project(${PROJECT_NAME})

add_subdirectory(Editor)
add_subdirectory(Engine)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Editor)