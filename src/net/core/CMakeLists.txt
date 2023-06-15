set(INCLUDE
  
  ${SRC_UTIL_DIR}/resource_limit
  )
include_directories(${INCLUDE})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRCS)

add_libraries(net_core ${SRCS})
