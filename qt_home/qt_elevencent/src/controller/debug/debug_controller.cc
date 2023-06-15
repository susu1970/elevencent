#include"debug_controller.h"
#include"app.h"
using namespace std;
using namespace qt_elevencent;

DebugController::DebugController(QObject*parent):BaseController(parent){
  m_debugV=new DebugView(App::getInstance()->m_appV);
  App::getInstance()->m_appV->m_vLayout->addWidget(m_debugV);
  m_debugV->hide();
}
bool DebugController::appEvent(AppEvent*ev){
  if(ev->type()==EVENT_TYPE_DEBUG_VIEW_SHOW){
    for(auto&w:App::getInstance()->m_appV->children())
      ((QWidget*)w)->hide();
    m_debugV->raise();
    m_debugV->show();
    return true;
  }else if(ev->type()==EVENT_TYPE_DEBUG_VIEW_HIDE){
    m_debugV->hide();
    return true;
  }
  return false;
}

