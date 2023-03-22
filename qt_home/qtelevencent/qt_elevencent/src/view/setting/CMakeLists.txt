set(SRC_VIEW_SETTING_SIGN_UP_DIR ${SRC_VIEW_SETTING_DIR}/sign_up)
add_subdirectory(${SRC_VIEW_SETTING_SIGN_UP_DIR})

set_global(G_INCLUDE ${G_INCLUDE}  ${SRC_VIEW_SETTING_DIR})
include_directories(${G_INCLUDE})
append_glob_src()

