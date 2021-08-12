set(PTHREAD_SRCS pthread.cc ${SRCS})
set(CREATETASKHANDLER_SRCS createTaskHandler.cc)

add_executable(createTaskHandler  ${CREATETASKHANDLER_SRCS})
add_executable(pthread  ${PTHREAD_SRCS})

target_link_libraries(pthread -lpthread -lcppunit)
target_link_libraries(createTaskHandler -lthread_pool -lpthread -lcppunit)
