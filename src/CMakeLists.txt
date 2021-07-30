set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SRC_BIN_DIR})

set(SRC_INCLUDE
  ${SRC_DIR}/global)
include_directories(${SRC_INCLUDE})

set(SRC_SUBDIRS
  util)
add_subdirectory(${SRC_SUBDIRS})
