set(SRC_MODEL_CMD_DIR ${SRC_MODEL_DIR}/cmd)
set(SRC_MODEL_NET_DIR ${SRC_MODEL_DIR}/net)
set(SRC_MODEL_TOAST_DIR ${SRC_MODEL_DIR}/toast)
set(SRC_MODEL_MSG_DIR ${SRC_MODEL_DIR}/msg)

add_subdirectory(${SRC_MODEL_CMD_DIR})
add_subdirectory(${SRC_MODEL_NET_DIR})
add_subdirectory(${SRC_MODEL_TOAST_DIR})
add_subdirectory(${SRC_MODEL_MSG_DIR})
