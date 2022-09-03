aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRCS)

add_executable(spinlock ${SRCS})

target_link_libraries(spinlock -lpthread -lthread_pool -lpthread)
