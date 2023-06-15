#ifndef CMD_CONTROLLER_H_
#define CMD_CONTROLLER_H_
#include"base_controller.h"
#include"cmd_view.h"
#include"comp_content_scroll.hpp"
#include<list>
#include<unordered_map>
#include<string>
#include<cmd_model.h>
namespace qt_elevencent{
  class CmdController:public BaseController{
    Q_OBJECT
  public:
    explicit CmdController(QObject*parent=0);
    bool appEvent(AppEvent*ev);
  private:
    
  public:
    CmdView*m_cmdV;
  };
}

#endif
