#ifndef APP_CONTROLLER_H_
#define APP_CONTROLLER_H_

#include"event.h"
#include"base_controller.h"
#include"home_view.h"

namespace qt_elevencent{
  class AppController:public BaseController{
    Q_OBJECT
  public:
    explicit AppController(QObject*parent=0);
    bool appEvent(AppEvent*ev);
  public:
    HomeView*m_hv;
  };
}

#endif
