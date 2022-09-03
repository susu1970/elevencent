
add_executable(rsa rsa.cc)

target_link_libraries(rsa -lrsa -lgmp -lgmpxx )
