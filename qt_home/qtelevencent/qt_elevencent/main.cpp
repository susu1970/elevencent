#include"app.h"
#include<QApplication>
using namespace qt_elevencent;
int main(int argc, char *argv[]){
  QApplication qapp(argc,argv);
  return App::getInstance()->run(&qapp);
}
