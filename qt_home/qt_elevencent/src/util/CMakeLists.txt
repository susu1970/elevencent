if(QT_PLANTFORM STREQUAL "android_arm64_v8a")
  set_global(G_ANDROID_EXTRA_LIBRARIES ${G_ANDROID_EXTRA_LIBRARIES}
    ${SRC_UTIL_DIR}/third_party/arm64_v8a/gmp_arm64_v8a/libgmp.so
    ${SRC_UTIL_DIR}/third_party/arm64_v8a/gmp_arm64_v8a/libgmpxx.so
    )
  set_global(G_INCLUDE ${G_INCLUDE}
    ${SRC_UTIL_DIR}/third_party/arm64_v8a/boost_1_74_arm64_v8a/include/boost-1_74/
    ${SRC_UTIL_DIR}/third_party/arm64_v8a/gmp_arm64_v8a/
    )
elseif(QT_PLANTFORM STREQUAL "gcc_64")
  set_global(G_PROJECT_LIBRARIES ${G_PROJECT_LIBRARIES}
    -lgmp -lgmpxx )
endif()
set_global(G_INCLUDE ${G_INCLUDE} ${SRC_UTIL_DIR})
include_directories(${G_INCLUDE})
append_glob_src()

