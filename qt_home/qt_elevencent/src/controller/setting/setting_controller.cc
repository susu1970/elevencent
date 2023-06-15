#include"setting_controller.h"
#include"app.h"
using namespace std;
using namespace qt_elevencent;
using namespace boost::program_options;
extern string program_options2str(options_description*desc);
SettingController::SettingController(QObject*parent):BaseController(parent){}
bool SettingController::appEvent(AppEvent*ev){
  switch(ev->type()){
  default:
    break;
  }
  return false;
}
