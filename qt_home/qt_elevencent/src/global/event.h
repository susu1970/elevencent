#ifndef _EVENT_H_
#define _EVENT_H_

#include<QEvent>
#include"global.h"

namespace qt_elevencent{

  enum EVENT_TYPE{
    EVENT_TYPE_START=QEvent::Type(QEvent::User),

    EVENT_TYPE_APP_RUN,
    EVENT_TYPE_HOME_VIEW_SHOW,
    EVENT_TYPE_DEBUG_VIEW_SHOW,
    EVENT_TYPE_DEBUG_VIEW_HIDE,
    EVENT_TYPE_CMD_VIEW_SHOW,
    EVENT_TYPE_CMD_VIEW_HIDE,
    EVENT_TYPE_CMD_VIEW_HINT_MSG,
    EVENT_TYPE_CMD_VIEW_OUTPUT,
    EVENT_TYPE_CMD_MAIN,
    EVENT_TYPE_CMD_LINE,    
    EVENT_TYPE_NET_REQ_PUBKEY,
    EVENT_TYPE_NET_TX,    
    EVENT_TYPE_TOAST_SHOW,
    EVENT_TYPE_GET_CONNED_SERVER,    
    
    EVENT_TYPE_MAX
  };
  class AppEvent:public QEvent{
  public:
    void*data;
    AppEvent(EVENT_TYPE type,void*pdata=0):QEvent(QEvent::Type(type)),data(pdata){}
  };
  
}


#endif
