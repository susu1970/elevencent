set(POOL_SRCS mariadb_pool.cc ${SRCS})

add_executable(mariadb_pool ${POOL_SRCS})

target_link_libraries(mariadb_pool -lmariadb_pool -lmariadbcpp -lpthread -lthread_pool)
