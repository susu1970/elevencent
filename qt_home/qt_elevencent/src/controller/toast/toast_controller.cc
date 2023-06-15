#include"toast_controller.h"
#include"toast_model.h"
#include"cmd_model.h"
#include"global.h"
#include"app.h"
using namespace std;
using namespace qt_elevencent;
using namespace boost::program_options;
extern string program_options2str(options_description*desc);
ToastController::ToastController(QObject*parent):BaseController(parent){  
  m_toast=new CompToast(App::getInstance()->m_appV);
  shared_ptr<options_description>toast_show_desc=make_shared<options_description>("show toast\n");
  toast_show_desc->add_options()
    ("str,s",value<string>(),"text")
    ("time,t",value<int>()->default_value(4000),"toast time");
  registeCmd({"toast","show"},[](int argc,char**argv,int match,void*pctl){
    int sub_argc=argc-match+1;
    ToastController*ctl=(ToastController*)pctl;
    if(!ctl||sub_argc<=0||match<1)
      return;
    char**sub_argv=&(argv[match-1]);
    auto pair=ctl->getCmdPair(argc,argv,match);
    if(!pair)return;
    auto desc=pair->desc;
    if(!desc)return;
    variables_map vm;
    string help=program_options2str(desc.get());
    try{
      store(parse_command_line(sub_argc,sub_argv,*desc),vm);
    }catch(const error&ex){
      CmdViewOutputArg outArg(CMD_VIEW_CONTENT_IDX::HINT,QString::fromStdString(string("toast show error\n")+ex.what()+string("\n")));
      App::getInstance()->sendEvent(EVENT_TYPE_CMD_VIEW_OUTPUT,&outArg);
      return;
    }
    string str=vm.count("str")?vm["str"].as<string>():"";
    int time=vm.count("time")?vm["time"].as<int>():4000;
    ctl->m_toast->show(QString::fromStdString(str),time);
  },toast_show_desc);
}
bool ToastController::appEvent(AppEvent*ev){
  switch(ev->type()){
  case EVENT_TYPE_TOAST_SHOW:{
    ToastShowArg*arg=(ToastShowArg*)ev->data;
    if(!arg)return true;
    m_toast->show(arg->m_text,arg->m_time);    
    return true;
  }
    break;
  default:
    break;
  }
  return false;
}
