set(INCLUDE
  ${SRC_DB_DIR}/mapper
  )
include_directories(${INCLUDE})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRCS)
add_libraries(db_mem_cache ${SRCS})
