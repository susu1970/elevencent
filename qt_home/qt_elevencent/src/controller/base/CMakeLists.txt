if(QT_PLANTFORM STREQUAL "gcc_64")
  find_package(Boost COMPONENTS program_options REQUIRED)
  set_global(G_PROJECT_LIBRARIES ${G_PROJECT_LIBRARIES} -lboost_program_options)
endif()

append_glob_src()
set_global(G_INCLUDE ${G_INCLUDE}  ${SRC_CONTROLLER_BASE_DIR})
include_directories(${G_INCLUDE})
