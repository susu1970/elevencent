set(SRC_NET_DIR ${SRC_RESOURCE_DIR}/net)

set(INCLUDE
  ${SRC_UTIL_DIR}/pool/thread_pool)
include_directories(${INCLUDE})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRCS)
add_executable(server ${SRCS})
target_link_libraries(server -lpthread -lthread_pool -lmariadb_resource -lINIReader -ldb_memory_cache -lmariadbcpp -lmariadb_pool)
