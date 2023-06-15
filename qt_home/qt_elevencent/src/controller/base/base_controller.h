#ifndef BASE_CONTROLLER_H_
#define BASE_CONTROLLER_H_

#include"event.h"
#include"cmd_model.h"
#include<QObject>
#include<list>
#include<string>
#include<unordered_map>
#include<initializer_list>
#include<boost/program_options.hpp>
#include<ostream>
#include<memory>
namespace qt_elevencent{

  class BaseController:public QObject{
    Q_OBJECT
  public:
    class CmdFuncPair{
    public:
      std::list<std::string>cmd;
      std::shared_ptr<boost::program_options::options_description>desc;
      std::function<void(int argc,char**argv,int match,void*)>func;
      CmdFuncPair(std::list<std::string>&&cmd_,std::shared_ptr<boost::program_options::options_description>desc_,std::function<void(int argc,char**argv,int match,void*)>&&func_):cmd(std::forward<std::list<std::string>>(cmd_)),func(std::forward<std::function<void(int argc,char**argv,int match,void*)>>(func_)),desc(desc_){}
    };
    std::list<CmdFuncPair>m_registeCmds;    
  public:    
    explicit BaseController(QObject*parent=0);
    bool eventFilter(QObject*obj,QEvent*ev);
    int matchCmd(int argc,char**argv);
    const CmdFuncPair*getCmdPair(int argc,char**argv,int match);
    void doCmd(int argc,char**argv,int match);
    virtual bool appEvent(AppEvent*ev);
    void registeCmd(std::initializer_list<std::string>&&cmds,std::function<void(int argc,char**argv,int match,void*)>&&func=[](int argc,char**argv,int match,void*ctl){},std::shared_ptr<boost::program_options::options_description>desc=0);
  };
}

#endif
