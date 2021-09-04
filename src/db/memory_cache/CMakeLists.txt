set(INCLUDE
  ${SRC_RESOURCE_DIR} ${SRC_DB_DIR}/mapper)
include_directories(${INCLUDE})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRCS)
add_libraries(db_memory_cache ${SRCS})
