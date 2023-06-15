#ifndef DEBUG_CONTROLLER_H_
#define DEBUG_CONTROLLER_H_

#include"base_controller.h"
#include"debug_view.h"
namespace qt_elevencent{
  class DebugController:public BaseController{
    Q_OBJECT
  public:
    explicit DebugController(QObject*parent=0);
    bool appEvent(AppEvent*ev);
  public:
    DebugView*m_debugV;
  };

}

#endif
