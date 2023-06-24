if(QT_PLANTFORM STREQUAL "android_arm64_v8a")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${SRC_UTIL_DIR}/third_party/arm64_v8a/boost_1_82_arm64_v8a/lib/ ${SRC_UTIL_DIR}/third_party/arm64_v8a/gmp_arm64_v8a/ -lboost_program_options-clang-mt-a64-1_82 -lgmp -lgmpxx")
  link_directories(${SRC_UTIL_DIR}/third_party/arm64_v8a/boost_1_82_arm64_v8a/lib/ ${SRC_UTIL_DIR}/third_party/arm64_v8a/gmp_arm64_v8a/)  
  set_global(G_INCLUDE ${G_INCLUDE}
    ${SRC_UTIL_DIR}/third_party/arm64_v8a/boost_1_82_arm64_v8a/include/boost-1_82/
    ${SRC_UTIL_DIR}/third_party/arm64_v8a/gmp_arm64_v8a/
    )
#  set_global(G_PROJECT_LIBRARIES ${G_PROJECT_LIBRARIES} -lboost_program_options-clang-mt-a64-1_82) 
#  link_directories(
#    ${SRC_UTIL_DIR}/third_party/arm64_v8a/boost_1_82_arm64_v8a/lib/
#    ${SRC_UTIL_DIR}/third_party/arm64_v8a/gmp_arm64_v8a/)
elseif(QT_PLANTFORM STREQUAL "gcc_64")
  set_global(G_PROJECT_LIBRARIES ${G_PROJECT_LIBRARIES} -lgmp -lgmpxx )
endif()
set_global(G_INCLUDE ${G_INCLUDE} ${SRC_UTIL_DIR})
include_directories(${G_INCLUDE})
append_glob_src()

