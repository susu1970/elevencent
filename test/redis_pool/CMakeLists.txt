set(POOL_SRCS redis_pool.cc ${SRCS})

add_executable(redis_pool ${POOL_SRCS})

target_link_libraries(redis_pool -lredis_pool -lhiredis -lpthread -lthread_pool)
