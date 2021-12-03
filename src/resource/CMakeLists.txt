set(SRC_RESOURCE_MARIADB_DIR ${SRC_RESOURCE_DIR}/mariadb)

set(INCLUDE
  ${SRC_DB_DIR}/mapper ${SRC_DB_DIR}/memory_cache)
include_directories(${INCLUDE})

add_subdirectory(mariadb)
