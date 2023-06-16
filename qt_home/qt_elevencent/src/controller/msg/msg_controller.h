#ifndef MSG_CONTROLLER_H_
#define MSG_CONTROLLER_H_

#include"base_controller.h"
#include"msg_model.h"
#include"message_view.h"
namespace qt_elevencent{
  class MsgController:public BaseController{
    Q_OBJECT
  public:
    explicit MsgController(QObject*parent=0);
  public:
    MsgModel*m_msgM;
    MsgView*m_msgV;
    bool appEvent(AppEvent*ev);
    void msgUsersShowByModel();
    void clearMsg();
  };
}

#endif
