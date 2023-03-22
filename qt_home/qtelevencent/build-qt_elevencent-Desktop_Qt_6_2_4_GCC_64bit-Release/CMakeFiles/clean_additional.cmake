# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles/qt_elevencent_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/qt_elevencent_autogen.dir/ParseCache.txt"
  "qt_elevencent_autogen"
  )
endif()
