set(SRC_NET_CORE_DIR ${SRC_NET_DIR}/core)
set(SRC_NET_PROTOCOL_DIR ${SRC_NET_DIR}/protocol)
set(SRC_NET_PROCESS_DIR ${SRC_NET_DIR}/process)
set(SRC_NET_SERVER_DIR ${SRC_NET_DIR}/server)

set(INCLUDE
  ${SRC_NET_CORE_DIR})
include_directories(${INCLUDE})

add_subdirectory(core)
add_subdirectory(process)
add_subdirectory(server)

