#include"app_controller.h"
#include"app.h"
#include"rsa.hpp"
#include <sys/resource.h>
#include<QTextCodec>
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
    srand(time(nullptr));	    
    App::getInstance()->m_appV->show();
    App::getInstance()->showWidget(m_hv);
    //generate rsakey
    RSA::Key pubkey,privkey;
    RSA::randomKey(pubkey,privkey);
    g_pubkey=RSA::key2str(pubkey);
    g_privkey=RSA::key2str(privkey);
    g_keypub=RSA::str2key(g_pubkey);
    g_keypriv=RSA::str2key(g_privkey);
    App::getInstance()->sendEvent(EVENT_TYPE_ON_HOME_MSG_VIEW_CREATE,m_hv->m_hcv->m_msgV);
    struct rlimit sLimit={
      .rlim_cur=RLIM_INFINITY,
      .rlim_max=RLIM_INFINITY,
    };
    setrlimit(RLIMIT_STACK,&sLimit);
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

