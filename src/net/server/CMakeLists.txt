set(INCLUDE
  ${SRC_UTIL_DIR}/pool/thread_pool
  ${SRC_NET_STRATEGY_DIR})
include_directories(${INCLUDE})
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRCS)
add_executable(server ${SRCS})
target_link_libraries(server -lpthread -lthread_pool -lnet_connection_strategy -lnet_core)
