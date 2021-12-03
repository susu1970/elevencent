set(INCLUDE
  ${SRC_DB_DIR}/mapper ${SRC_DB_DIR}/memory_cache ${SRC_UTIL_DIR}/pool/db_pool/mariadb ${SRC_RESOURCE_DIR})
include_directories(${INCLUDE})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRCS)
add_libraries(mariadb_resource ${SRCS})
