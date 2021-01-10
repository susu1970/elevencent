include_directories(
  /usr/local/include/
  ${PROJECT_SOURCE_DIR}/test/
  )

set(LINK_DIRECTORIES
#  /usr/local/lib
  /usr/lib/x86_64-linux-gnu/
  )

FILE(GLOB_RECURSE CC_TEST_FILES ${CMAKE_SOURCE_DIR}/src/*.cc  ${CMAKE_SOURCE_DIR}/src/*.cpp)

link_directories(${LINK_DIRECTORIES})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lcppunit")

add_executable(global global.cc)
#add_executable(cppunit cppunit.cc)
add_executable(pthread pthread.cc)
#add_executable(thread_pool_node thread_pool_node.cc ${CC_TEST_FILES})
add_executable(thread_pool thread_pool.cc ${CC_TEST_FILES})
