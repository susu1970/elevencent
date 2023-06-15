set(INCLUDE
  ${SRC_DB_DIR}/mapper ${SRC_DB_DIR}/mem_cache ${SRC_UTIL_DIR}/pool/db_pool/mariadb)
include_directories(${INCLUDE})
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRCS)
add_libraries(resource ${SRCS})
