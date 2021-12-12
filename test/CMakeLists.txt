set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN_DIR})

set(TEST_INCLUDE
  ${SRC_DIR}/global
  ${SRC_DIR}/util/pool/thread_pool
  ${SRC_DIR}/util/pool/db_pool/mariadb
  ${SRC_DIR}/util/pool/redis_pool
  ${SRC_DIR}/resource/mariadb
  ${SRC_DIR}/db/mapper
  ${SRC_DIR}/db/memory_cache
  ${SRC_DIR}/resource
  ${TEST_DIR})
include_directories(${TEST_INCLUDE})

add_subdirectory(thread_pool)
add_subdirectory(ini_parser)
add_subdirectory(global)
add_subdirectory(mariadb_pool)
add_subdirectory(redis_pool)
add_subdirectory(mariadb_resource)
#add_subdirectory(mapper)
#add_subdirectory(memory_cache)
