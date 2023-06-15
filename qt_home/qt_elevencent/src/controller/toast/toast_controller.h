#ifndef TOAST_CONTROLLER_H_
#define TOAST_CONTROLLER_H_

#include"base_controller.h"
#include"comp_toast.hpp"

namespace qt_elevencent{
  class ToastController:public BaseController{
    Q_OBJECT
  public:
    explicit ToastController(QObject*parent=0);
  public:
    CompToast*m_toast;
    bool appEvent(AppEvent*ev);
  };
}

#endif
