aux_source_directory(${SRC_DIR}/util/pool/thread_pool SRCS)

set(PTHREAD_SRCS pthread.cc ${SRCS})
set(CREATETASKHANDLER_SRCS createTaskHandler.cc  ${SRCS})

add_executable(createTaskHandler  ${CREATETASKHANDLER_SRCS})
add_executable(pthread  ${PTHREAD_SRCS})

target_link_libraries(pthread -lpthread -lcppunit)
target_link_libraries(createTaskHandler -lpthread -lcppunit -lthread_pool)
