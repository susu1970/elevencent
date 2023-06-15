#include"msg_controller.h"
#include"cmd_model.h"
#include"global.h"
#include"app.h"
using namespace std;
using namespace qt_elevencent;
using namespace boost::program_options;
extern string program_options2str(options_description*desc);
MsgController::MsgController(QObject*parent):BaseController(parent){  
}
bool MsgController::appEvent(AppEvent*ev){
  return false;
}
