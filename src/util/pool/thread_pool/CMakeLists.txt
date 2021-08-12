aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRCS)
add_libraries(thread_pool ${SRCS})
#target_link_libraries(libthread_pool.so pthread)
