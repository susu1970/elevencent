#ifndef CONTROLLERS_H_
#define CONTROLLERS_H_
#include"app_controller.h"
#include"net_controller.h"
#include"cmd_controller.h"
#include"toast_controller.h"
#include"setting_controller.h"
namespace qt_elevencent{
  enum CONTROLLER_IDX{
    CONTROLLER_IDX_START=0,
    
    CONTROLLER_IDX_APP,
    CONTROLLER_IDX_NET,
    CONTROLLER_IDX_CMD,
    CONTROLLER_IDX_TOAST,
    CONTROLLER_IDX_SETTING,

    CONTROLLER_IDX_MAX
  };
}

#endif
