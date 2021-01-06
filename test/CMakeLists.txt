include_directories(/usr/local/include/)

set(LINK_DIRECTORIES
  /usr/local/lib
  /usr/lib/x86_64-linux-gnu/
  )
link_directories(${LINK_DIRECTORIES})

add_executable(global global.cc)
add_executable(cppunit cppunit.cc)
add_executable(pthread pthread.cc)

target_link_libraries(cppunit ${LINK_DIRECTORIES}/libcppunit.a)


