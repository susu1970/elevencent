cmake_minimum_required(VERSION 3.21.1)

project(qt_elevencent VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_VERSION_MAJOR 6)



find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Core5Compat)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(TEST_DIR ${PROJECT_SOURCE_DIR}/test)
set(TARGET_DIR ${PROJECT_SOURCE_DIR}/target)
set(BIN_DIR ${TARGET_DIR}/bin)
set(SRC_BIN_DIR ${BIN_DIR}/src)
set(TEST_BIN_DIR ${BIN_DIR}/test)
set(LIB_DIR ${TARGET_DIR}/lib)
set(LIB_SHARED_DIR ${LIB_DIR}/shared)
set(LIB_STATIC_DIR ${LIB_DIR}/static)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_STATIC_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_SHARED_DIR})
add_definitions("-g ")
if(QT_PLANTFORM STREQUAL "android_arm64_v8a")
  add_definitions("-w ")
  set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -L${SRC_DIR}//util/third_party/arm64_v8a/gmp_arm64_v8a/ -L${SRC_DIR}/util/third_party/arm64_v8a/boost_1_74_arm64_v8a/lib/   -lgmp -lgmpxx  -lboost_program_options-clang-mt-a64-1_74")  
  message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
endif()

link_directories(${LIB_SHARED_DIR} ${LIB_STATIC_DIR})

function(add_shared_library)
  set(oldname ${ARGV0})
  set(libname ${ARGV0}_shared)
  set(files)
  math(EXPR ARGCM1 "${ARGC} - 1" OUTPUT_FORMAT DECIMAL)
  foreach(i RANGE 1 ${ARGCM1} 1)
    set(files ${files} ${ARGV${i}})
  endforeach(i)
  add_library(${libname} SHARED ${files})
  SET_TARGET_PROPERTIES(${libname} PROPERTIES CLEAN_DIRECT_OUTPUT 1)
  SET_TARGET_PROPERTIES(${libname} PROPERTIES OUTPUT_NAME ${oldname})  
endfunction(add_shared_library)

function(add_static_library)
  set(oldname ${ARGV0})
  set(libname ${ARGV0}_static)
  set(files)
  math(EXPR ARGCM1 "${ARGC} - 1" OUTPUT_FORMAT DECIMAL)
  foreach(i RANGE 1 ${ARGCM1} 1)
    set(files ${files} ${ARGV${i}})
  endforeach(i)
  add_library(${libname} STATIC ${files})
  SET_TARGET_PROPERTIES(${libname} PROPERTIES CLEAN_DIRECT_OUTPUT 1)
  SET_TARGET_PROPERTIES(${libname} PROPERTIES OUTPUT_NAME ${oldname})
endfunction(add_static_library)

macro(add_libraries)
  add_shared_library(${ARGV})
  add_static_library(${ARGV})
endmacro(add_libraries)
function(set_global)
  set(global_name ${ARGV0})
  set(append_str)
  math(EXPR ARGCM1 "${ARGC} - 1" OUTPUT_FORMAT DECIMAL)  
  foreach(i RANGE 1 ${ARGCM1} )
    set(append_str ${append_str} ${ARGV${i}})
  endforeach(i)
  set(${global_name}
    ${append_str}
    CACHE INTERNAL "")
endfunction(set_global)
macro(append_glob_src)
  file(GLOB ALL_SRC "*.cc" "*.cpp" "*.h" "*.hpp")
  set_global(G_PROJECT_SOURCES
    ${G_PROJECT_SOURCES}
    ${ALL_SRC}
    )
endmacro(append_glob_src)
macro(append_glob_link)
  set(append_str)
  math(EXPR ARGCM1 "${ARGC} - 1" OUTPUT_FORMAT DECIMAL)  
  foreach(i RANGE 1 ${ARGCM1} )
    set(append_str ${append_str} ${ARGV${i}})
  endforeach(i)  
  set_global(G_PROJECT_LIBRARIES
    ${G_PROJECT_LIBRARIES}
    ${append_str}
    )
endmacro(append_glob_link)

set(G_INCLUDE "" CACHE INTERNAL "global include" )
set(G_PROJECT_LIBRARIES "" CACHE INTERNAL "global link libraries" )
set(G_ANDROID_EXTRA_LIBRARIES "" CACHE INTERNAL "global android extra libraries" )
set(G_PROJECT_SOURCES
  main.cpp
  CACHE INTERNAL "global link libraries"   
  )

add_subdirectory(${SRC_DIR})
include_directories(${G_INCLUDE})

qt_add_executable(qt_elevencent
  MANUAL_FINALIZATION
  ${G_PROJECT_SOURCES}
  )

target_link_libraries(qt_elevencent PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Core5Compat ${G_PROJECT_LIBRARIES})

set_target_properties(qt_elevencent PROPERTIES
  MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
  MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
  MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
  MACOSX_BUNDLE TRUE
  WIN32_EXECUTABLE TRUE
  QT_ANDROID_EXTRA_LIBS "${G_ANDROID_EXTRA_LIBRARIES}"  
  )

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(qt_elevencent)
endif()
