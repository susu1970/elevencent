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
  App::getInstance()->addWidget(m_msgContentV=new MsgContentView);
  connect(m_msgContentV->m_topV->m_back,&QPushButton::clicked,[](){
    App::getInstance()->sendEvent(EVENT_TYPE_HOME_VIEW_SHOW);
  });
  connect(m_msgContentV->m_sendMsg->m_btn,&QPushButton::clicked,[this](){
    QString user=m_msgContentV->m_topV->m_label->text();
    user.replace("\\","\\\\");
    user.replace("\"","\\\"");
    QString msg=m_msgContentV->m_sendMsg->m_lineEdit->text();
    msg.replace("\\","\\\\");
    msg.replace("\"","\\\"");    
    QString cmd=QString("net user_resource sendmsg -n \"")+user+"\" -m \""+msg+"\"";
    CmdLineModel cmdModel(cmd);
    App::getInstance()->sendEvent(EVENT_TYPE_CMD_LINE,&cmdModel);
    reqMsgContent();
    msgContentShowByModel();    
  });
  m_reqMsgContentTimer=new QTimer(this);
  connect(m_reqMsgContentTimer,SIGNAL(timeout()),this,SLOT(reqMsgContent()));
  m_reqMsgContentTimer->start(3000);
}
void MsgController::reqMsgContent(){
  NetModel::Server::Host curhost;
  App::getInstance()->sendEvent(EVENT_TYPE_GET_CONNED_SERVER,&curhost);    
  if(!curhost.isConn||curhost.rsaKeypub.str.empty()||!curhost.isLogin)
    return;
  if(m_msgContentV->isHidden())
    return;
  QString name=m_msgContentV->m_topV->m_label->text();
  name.replace("\\","\\\\");
  name.replace("\"","\\\"");
  QString from="0";
  if(!(m_msgM->userMsg[m_msgContentV->m_topV->m_label->text().toStdString()]).empty()){
    auto&msgUnit=*(m_msgM->userMsg[m_msgContentV->m_topV->m_label->text().toStdString()].rbegin());
    from=QString::number(msgUnit.update_time);
  }    
  QString reqmsg_cmdstr=QString("net user_resource reqmsg -n \"")+name+"\" -f "+from;
  CmdLineModel reqmsg_cmd(reqmsg_cmdstr);  
  App::getInstance()->sendEvent(EVENT_TYPE_CMD_LINE,&reqmsg_cmd);  
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
  case EVENT_TYPE_RESP_USER_REQ_MSG:{
    MsgModel::EventRespUserReqMsg*data=(MsgModel::EventRespUserReqMsg*)ev->data;
    m_msgM->insertMsg(data->name,data->msgUnit);
    msgContentShowByModel();    
    return true;
  }
    break;
  case EVENT_TYPE_ON_HOME_MSG_VIEW_CREATE:{
    m_msgV=(MsgView*)ev->data;
    connect(m_msgV->m_contentV,SIGNAL(clicked(QModelIndex)),this,SLOT(onMsgUserClicked(QModelIndex)));    
    return true;
  }
    break;
  case EVENT_TYPE_ON_HOME_MSG_VIEW_SHOW:{
    CmdLineModel cmd("net user_resource reqmsg_users");
    App::getInstance()->sendEvent(EVENT_TYPE_CMD_LINE,&cmd);
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
  m_msgV->m_contentV->m_model->clearItem();
  m_msgV->m_contentV->update();
  m_msgContentV->m_listV->m_model->clearItem();
  m_msgContentV->update();
}
void MsgController::msgContentShowByModel(){
  int prevSize=m_msgContentV->m_listV->m_model->m_datas.size();
  m_msgContentV->m_listV->m_model->clearItem();
  for(auto iter=m_msgM->userMsg[m_msgContentV->m_topV->m_label->text().toStdString()].begin();iter!=m_msgM->userMsg[m_msgContentV->m_topV->m_label->text().toStdString()].end();++iter){
    m_msgContentV->m_listV->m_model->addItem(CompContentListItemData(QString::fromStdString((*iter).content),"",(void*)&(*iter)));    
  }
  if(m_msgContentV->m_listV->m_model->m_datas.size()>prevSize)
    m_msgContentV->m_listV->verticalScrollBar()->setValue(m_msgContentV->m_listV->verticalScrollBar()->maximum());  
  m_msgContentV->update();
}
void MsgController::msgUsersShowByModel(){
  m_msgV->m_contentV->m_model->clearItem();  
  for(auto&e:m_msgM->msgUsers){
    m_msgV->m_contentV->m_model->addItem(CompContentListItemData(QString::fromStdString(e)));      
  }
  m_msgV->m_contentV->update();
}
void MsgController::onMsgUserClicked(QModelIndex idx){
  CompContentListItemData*itemData=m_msgV->m_contentV->m_model->m_datas[idx.row()];
  if(!itemData){
    return;
  }
  m_msgContentV->m_topV->m_label->setText(itemData->m_name);
  reqMsgContent();  
  msgContentShowByModel();
  App::getInstance()->showWidget(m_msgContentV);
}
