#ifndef SETTING_CONTROLLER_H_
#define SETTING_CONTROLLER_H_

#include"base_controller.h"
#include"event.h"
#include"net_thread.h"
#include"sign_up_view.h"

namespace qt_elevencent{

  class SettingController:public BaseController{
    Q_OBJECT
  public:
    explicit SettingController(QObject*parent=0);
    bool appEvent(AppEvent*ev);
  public:
    SignUpView*m_signupV;
  };
}

#endif
