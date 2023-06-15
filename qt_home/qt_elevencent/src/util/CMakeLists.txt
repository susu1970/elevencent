set_global(G_INCLUDE ${G_INCLUDE}  ${SRC_UTIL_DIR})
include_directories(${G_INCLUDE})
set_global(G_PROJECT_LIBRARIES ${G_PROJECT_LIBRARIES} -lgmp -lgmpxx )
append_glob_src()

