#include"base_controller.h"
#include"app.h"
using namespace std;
using namespace qt_elevencent;

BaseController::BaseController(QObject*parent):QObject(parent){
  App::getInstance()->installEventFilter(this);
}
bool BaseController::eventFilter(QObject*obj,QEvent*ev){
  return ((void*)obj==(void*)App::getInstance()->m_eventObj)?appEvent((AppEvent*)ev):false;
}
bool BaseController::appEvent(AppEvent*ev){
  return false;
}
int BaseController::matchCmd(int argc,char**argv){
  int match=0;
  for(auto cmds=m_registeCmds.begin();cmds!=m_registeCmds.end();++cmds){
    int i=0;
    for(auto cmd=cmds->cmd.begin();cmd!=cmds->cmd.end();++cmd,++i)
      if(*cmd!=argv[i])
	goto cmd_match_end;
  cmd_match_end:
    if(i>match)
      match=i;
  }
  return match;
}
const BaseController::CmdFuncPair*BaseController::getCmdPair(int argc,char**argv,int match){
  int max_match=0;
  CmdFuncPair*pair=0;
  for(auto cmds=m_registeCmds.begin();cmds!=m_registeCmds.end();++cmds){
    int i=0;
    for(auto cmd=cmds->cmd.begin();cmd!=cmds->cmd.end();++cmd,++i)
      if(*cmd!=argv[i])
	goto cmd_match_end;
  cmd_match_end:
    if(i>max_match){
      max_match=i;
      pair=&(*cmds);
    }
  }
  return pair;
}
void BaseController::doCmd(int argc,char**argv,int match){
  int max_match=0;
  std::function<void(int argc,char**argv,int match,void*)>*func=0;
  for(auto cmds=m_registeCmds.begin();cmds!=m_registeCmds.end();++cmds){
    int i=0;
    for(auto cmd=cmds->cmd.begin();cmd!=cmds->cmd.end();++cmd,++i)
      if(*cmd!=argv[i])
	goto cmd_match_end;
  cmd_match_end:
    if(i>max_match){
      max_match=i;
      func=&cmds->func;
    }
  }
  if(func)
    (*func)(argc,argv,match,this);
}
void BaseController::registeCmd(initializer_list<string>&&cmds,function<void(int argc,char**argv,int match,void*)>&&func,shared_ptr<boost::program_options::options_description>desc){
  if(!cmds.size())return;
  m_registeCmds.push_back(CmdFuncPair(forward<initializer_list<string>>(cmds),desc,forward<function<void(int argc,char**argv,int match,void*)>>(func)));
}
