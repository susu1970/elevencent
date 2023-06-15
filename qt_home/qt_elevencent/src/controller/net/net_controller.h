#ifndef NET_CONTROLLER_H_
#define NET_CONTROLLER_H_

#include"base_controller.h"
#include"net_protocol.hpp"
#include"event.h"
#include"net_thread.h"

namespace qt_elevencent{

  class NetController:public BaseController{
    Q_OBJECT
  public:
    explicit NetController(QObject*parent=0);
    ~NetController();
    NetThread m_thr;
  protected:
    bool appEvent(AppEvent*ev);    
  signals:
    void reconnect2server();
    void pushServer(NetModel::Server::Host h);
    void pushOut(NetModel::STATE_OUT state,NetModel::StateCtx*ctx);    
  };
}

#endif
