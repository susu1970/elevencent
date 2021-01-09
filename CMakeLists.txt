cmake_minimum_required(VERSION 3.0)
project(elevencent)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_BUILD_TYPE Debug)

message(STATUS "PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}")
message(STATUS "PROJECT_BINARY_DIR=${PROJECT_BINARY_DIR}")

include_directories(
  ${PROJECT_SOURCE_DIR}/src/global
  ${PROJECT_SOURCE_DIR}/src/utils
  )

enable_testing()

add_subdirectory(src bin)
add_subdirectory(test bin_test)

message("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
FILE(GLOB_RECURSE CC_FILES ${CMAKE_SOURCE_DIR}/src/*.cc  ${CMAKE_SOURCE_DIR}/src/*.cpp)
message("CC_FILES: ${CC_FILES}")
