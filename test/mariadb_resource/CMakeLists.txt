set(MARIADB_RESOURCE_SRCS test_mariadb_resource.cc ${SRCS})

add_executable(mariadb_resource ${MARIADB_RESOURCE_SRCS})

target_link_libraries(mariadb_resource -lmariadb_resource -lINIReader -ldb_memory_cache -lmariadbcpp -lmariadb_pool)
