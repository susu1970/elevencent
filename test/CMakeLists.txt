set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN_DIR})

set(TEST_INCLUDE
  ${SRC_DIR}/global
  ${SRC_DIR}/util/pool/thread_pool
  ${SRC_DIR}/util/pool/db_pool/mariadb
  ${TEST_DIR})
include_directories(${TEST_INCLUDE})

add_subdirectory(thread_pool)
add_subdirectory(ini_parser)
add_subdirectory(global)
add_subdirectory(mariadb_pool)
