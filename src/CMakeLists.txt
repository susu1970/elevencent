set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SRC_BIN_DIR})

set(SRC_GLOBAL_DIR ${SRC_DIR}/global)
set(SRC_DB_DIR ${SRC_DIR}/db)
set(SRC_RESOURCE_DIR ${SRC_DIR}/resource)
set(SRC_UTIL_DIR ${SRC_DIR}/util)
set(SRC_NET_DIR ${SRC_DIR}/net)

set(INCLUDE
  ${SRC_GLOBAL_DIR})
include_directories(${INCLUDE})

add_subdirectory(util)
add_subdirectory(db)
add_subdirectory(resource)
add_subdirectory(net)

