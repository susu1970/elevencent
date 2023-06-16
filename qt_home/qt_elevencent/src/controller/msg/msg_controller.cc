#include"msg_controller.h"
#include"msg_model.h"
#include"cmd_model.h"
#include"global.h"
#include"app.h"
using namespace std;
using namespace qt_elevencent;
using namespace boost::program_options;
extern string program_options2str(options_description*desc);
MsgController::MsgController(QObject*parent):BaseController(parent){
  m_msgM=new MsgModel;
  
}
bool MsgController::appEvent(AppEvent*ev){
  switch(ev->type()){
  case EVENT_TYPE_RESP_MSG_USERS:{
    MsgModel::EventRespMsgUsers*data=(MsgModel::EventRespMsgUsers*)ev->data;
    m_msgM->msgUsers.insert(data->name);
    msgUsersShowByModel();
    return true;
  }
    break;
  case EVENT_TYPE_ON_HOME_MSG_VIEW_CREATE:{
    m_msgV=(MsgView*)ev->data;
    return true;
  }
    break;
  case EVENT_TYPE_ON_HOME_MSG_VIEW_SHOW:{
    App::getInstance()->sendEvent(EVENT_TYPE_CMD_LINE,new CmdLineModel("net user_resource reqmsg_users"));
    msgUsersShowByModel();    
    return true;
  }
    break;
  case EVENT_TYPE_NET_ON_LOGIN_SUCCESS:{
    clearMsg();
    return false;
  }
    break;
  case EVENT_TYPE_NET_ON_DISCONNECTED:{
    clearMsg();
    return false;
  }
    break;
  default:
    break;
  }
  return false;
}
void MsgController::clearMsg(){
  m_msgM->clear();
  m_msgV->m_contentV->clearItem();
  m_msgV->m_contentV->update();
}
void MsgController::msgUsersShowByModel(){
  m_msgV->m_contentV->clearItem();  
  for(auto&e:m_msgM->msgUsers){
    m_msgV->m_contentV->addItem(CompContentListItemData(QString::fromStdString(e)));      
  }
  m_msgV->m_contentV->update();
}
