#include"app.h"
#include<iostream>
#include<QApplication>
using namespace std;
using namespace qt_elevencent;
App*App::ms_instance;
QMutex App::ms_instanceMutex;
App::App(){
  m_hv=new HomeView();
}
void App::show(){
  m_hv->show();
}
int App::run(QApplication*qapp){
  show();
  return qapp->exec();
}
App*App::getInstance(){
  if(ms_instance)
    return ms_instance;
  ms_instanceMutex.lock();
  if(!ms_instance)
    ms_instance=new App();
  ms_instanceMutex.unlock();
  return ms_instance;
}
App::~App(){
  delete m_hv;
}
