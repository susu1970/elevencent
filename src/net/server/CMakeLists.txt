set(INCLUDE
  ${SRC_UTIL_DIR}/pool/thread_pool
  ${SRC_UTIL_DIR}/rsa
  ${SRC_DB_DIR}/mapper
  ${SRC_RESOURCE_DIR}
  ${SRC_NET_PROCESS_DIR}
  ${SRC_NET_PROCESS_DIR}/tcp  
  ${SRC_NET_PROTOCOL_DIR})
include_directories(${INCLUDE})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRCS)
add_executable(server ${SRCS})

target_link_libraries(server  -lINIReader -lpthread -lthread_pool  -lgmp -lgmpxx -lrsa  -lnet_core  -lnet_process)
