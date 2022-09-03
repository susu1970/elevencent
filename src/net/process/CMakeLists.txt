set(INCLUDE
  ${SRC_UTIL_DIR}/rsa
  ${SRC_UTIL_DIR}/spinlock)
include_directories(${INCLUDE})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRCS)
add_libraries(net_process ${SRCS})

set(SRC_NET_PROCESS_TCP_DIR ${SRC_NET_PROCESS_DIR}/tcp)
