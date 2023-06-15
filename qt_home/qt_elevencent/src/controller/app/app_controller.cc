#include"app_controller.h"
#include"app.h"
#include"rsa.hpp"
using namespace std;
using namespace qt_elevencent;
string qt_elevencent::g_privkey="";
string qt_elevencent::g_pubkey="";
RSA::Key qt_elevencent::g_keypub;
RSA::Key qt_elevencent::g_keypriv;
AppController::AppController(QObject*parent):BaseController(parent){
  App::getInstance()->addWidget(m_hv=new HomeView());
}
bool AppController::appEvent(AppEvent*ev){
  switch(ev->type()){
  case EVENT_TYPE_APP_RUN:{
    App::getInstance()->m_appV->show();
    App::getInstance()->showWidget(m_hv);
    //generate rsakey
    RSA::Key pubkey,privkey;
    RSA::randomKey(pubkey,privkey);
    g_pubkey=RSA::key2str(pubkey);
    g_privkey=RSA::key2str(privkey);
    g_keypub=RSA::str2key(g_pubkey);
    g_keypriv=RSA::str2key(g_privkey);
    qDebug()<<"g_pubkey:\n"<<QString::fromStdString(g_pubkey)<<"\ng_privkey:\n"<<QString::fromStdString(g_privkey);

    return true;
  }
    break;
  case EVENT_TYPE_HOME_VIEW_SHOW:{
    App::getInstance()->showWidget(m_hv);
    return true;
  }
    break;
  default:
    break;
  }
  return false;
}

