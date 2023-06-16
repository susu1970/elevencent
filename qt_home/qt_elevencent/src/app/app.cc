#include"app.h"
#include"event.h"
#include"controllers.h"
using namespace std;
using namespace qt_elevencent;
App*App::ms_instance;
QMutex App::ms_instanceMutex;
void*createController(int idx,QObject*parent=0){
  void*c=0;
  switch(idx){
  case CONTROLLER_IDX_APP:
    c=new AppController(parent);
    break;
  case CONTROLLER_IDX_NET:
    c=new NetController(parent);      
    break;
  case CONTROLLER_IDX_CMD:
    c=new CmdController(parent);
    break;
  case CONTROLLER_IDX_TOAST:
    c=new ToastController(parent);
    break;
  case CONTROLLER_IDX_SETTING:
    c=new SettingController(parent);
    break;
  case CONTROLLER_IDX_MSG:
    c=new MsgController(parent);
    break;    
  default:
    break;
  }
  return c;
}
App::App(){}
int App::run(QApplication*qapp){
  m_qa=qapp;
  m_appV=new AppView();
  m_eventObj=new QObject(qapp);
  for(int i=CONTROLLER_IDX_START+1;i<CONTROLLER_IDX_MAX;++i)
    m_controllers[i]=createController(i);
  sendEvent(EVENT_TYPE_APP_RUN);
  return qapp->exec();
}
void App::installEventFilter(QObject*obj){
  m_eventObj->installEventFilter(obj);
}
void App::removeEventFilter(QObject*obj){
  m_eventObj->removeEventFilter(obj);
}
void App::sendEvent(EVENT_TYPE type,void*data){
  AppEvent ev(type,data);
  QApplication::sendEvent(m_eventObj,(QEvent*)&ev);
}
void App::postEvent(EVENT_TYPE type,void*data){
  QApplication::postEvent(m_eventObj,new AppEvent(type,data));
}
void App::addWidget(QWidget*w){
  w->setContentsMargins(0,0,0,0);
  w->setParent(m_appV);
  m_appV->m_vLayout->addWidget(w);
}
void App::showWidget(QWidget*w){
  for(auto&v:m_appV->children())
    ((QWidget*)v)->hide();
  w->show();
  w->raise();
}
App*App::getInstance(){
  if(ms_instance)
    return ms_instance;
  ms_instanceMutex.lock();
  if(!ms_instance)
    ms_instance=new App();
  ms_instanceMutex.unlock();
  return ms_instance;
}
