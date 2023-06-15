#include"cmd_controller.h"
#include"app.h"
#include"controllers.h"
#include"cmd_model.h"
#include<boost/program_options.hpp>
using namespace std;
using namespace qt_elevencent;
using namespace boost::program_options;
extern int str2arg(const QString&s,char*argv[]);
extern string program_options2str(options_description*desc);
CmdController::CmdController(QObject*parent):BaseController(parent){
  App::getInstance()->addWidget(m_cmdV=new CmdView);
  shared_ptr<options_description>clear_desc=make_shared<options_description>("clear, clear cmd view output area's contents\n");
  clear_desc->add_options()
    ("pos,p",value<string>()->default_value("all"),"clear position [all/hint/content]");
  registeCmd({"clear"},[](int argc,char**argv,int match,void*pctl){
    CmdController*ctl=(CmdController*)pctl;
    if(!ctl)return;
    const BaseController::CmdFuncPair*pair=ctl->getCmdPair(argc,argv,match);
    auto desc=pair->desc;
    if(!desc)return;
    variables_map vm;
    string help=program_options2str(desc.get());
    try{
      store(parse_command_line(argc,argv,*desc),vm);
    }catch(const error&ex){
      ctl->m_cmdV->output(CMD_VIEW_CONTENT_IDX::HINT,QString::fromStdString(ex.what()+string("\n")+help+"\n"));
      return;
    }
    string p=vm.count("pos")?vm["pos"].as<string>():"";
    if(p!="all"&&p!="hint"&&p!="content"){
      ctl->m_cmdV->output(CMD_VIEW_CONTENT_IDX::HINT,"unrecognized option [all/hint/content]:\n");
      ctl->m_cmdV->output(CMD_VIEW_CONTENT_IDX::HINT,QString::fromStdString(p+"\n"),true,"<font size='+1' color=#4285F4>","</font>");      
      ctl->m_cmdV->output(CMD_VIEW_CONTENT_IDX::HINT,QString::fromStdString(help));
      return;
    }
    if(p!="hint")
      ctl->m_cmdV->output(CMD_VIEW_CONTENT_IDX::CONTENT,"",false);
    if(p!="content")
      ctl->m_cmdV->output(CMD_VIEW_CONTENT_IDX::HINT,"",false);
  },clear_desc);
  registeCmd({"help"},[](int argc,char**argv,int match,void*pctl){
    CmdController*ctl=(CmdController*)pctl;
    if(!ctl)return;
    BaseController*bctl=0;
    for(int i=CONTROLLER_IDX_START+1;i<CONTROLLER_IDX_MAX;++i){
      if(!(bctl=(BaseController*)App::getInstance()->m_controllers[i]))
	continue;
      for(auto cmds=bctl->m_registeCmds.begin();cmds!=bctl->m_registeCmds.end();++cmds){
	auto desc=cmds->desc;
	auto cmd=cmds->cmd;
	string help="";
	for(auto&e:cmd)
	  help+=e+"\t";
	help+="\n";
	if(desc)
	  help+=program_options2str(desc.get())+"\n";
	CmdViewOutputArg arg(CMD_VIEW_CONTENT_IDX::CONTENT,QString::fromStdString(help));
	App::getInstance()->sendEvent(EVENT_TYPE_CMD_VIEW_OUTPUT,&arg);
      }
    }    
  });
  registeCmd({"gohome"},[](int argc,char**argv,int match,void*pctl){
    App::getInstance()->sendEvent(EVENT_TYPE_HOME_VIEW_SHOW);
  });  
}
bool CmdController::appEvent(AppEvent*ev){
  switch(ev->type()){
  case EVENT_TYPE_CMD_VIEW_SHOW:
    App::getInstance()->showWidget(m_cmdV);
    return true;
    break;
  case EVENT_TYPE_CMD_VIEW_HIDE:
    m_cmdV->hide();
    return true;
    break;
  case EVENT_TYPE_CMD_MAIN:{
    CmdModel*cmd=(CmdModel*)ev->data;
    if(!cmd)return true;
    int max_match=0;
    BaseController*ctl=0,*match_ctl=0;
    for(int i=CONTROLLER_IDX_START+1;i<CONTROLLER_IDX_MAX;++i){
      if(!(ctl=(BaseController*)App::getInstance()->m_controllers[i]))
	continue;
      int match=ctl->matchCmd(cmd->argc,cmd->pargv);
      if(match>max_match){
	max_match=match;
	match_ctl=ctl;
      }
    }    
    if(!match_ctl){
      m_cmdV->output(CMD_VIEW_CONTENT_IDX::HINT,"there are no match controller to handle cmd:\n");
      return true;
    }
    match_ctl->doCmd(cmd->argc,cmd->pargv,max_match);
    return true;
  }
  case EVENT_TYPE_CMD_VIEW_OUTPUT:{
    CmdViewOutputArg*arg=(CmdViewOutputArg*)ev->data;
    if(!arg)return true;
    m_cmdV->output(forward<CmdViewOutputArg>(*arg));
    return true;
  }
    break;
  case EVENT_TYPE_CMD_LINE:{
    CmdLineModel*line=(CmdLineModel*)ev->data;
    CmdModel cmd;
    for(int i=0;i<MAX_CMD_ARGC;++i)
      cmd.pargv[i]=cmd.argv[i];
    cmd.argc=str2arg(line->cmdline,cmd.pargv);
    App::getInstance()->sendEvent(EVENT_TYPE_CMD_MAIN,&cmd);
  }
    break;
  default:
    break;
  }
  return false;
}

