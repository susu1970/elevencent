#ifndef APP_H_
#define APP_H_
#include<QMutex>
#include<QRect>
#include<QApplication>
#include"event.h"
#include"app_view.hpp"
#include"controllers.h"
namespace qt_elevencent{
  class App{
  private:
    ~App();  
    App();
    App(const App&)=delete;
    const App&operator=(const App&)=delete;
    static App*ms_instance;
    static QMutex ms_instanceMutex;
  public:
    QObject*m_eventObj;
    AppView*m_appV;
    QApplication*m_qa;    
    void*m_controllers[CONTROLLER_IDX_MAX]={0};
    int run(QApplication*qapp);
    void sendEvent(EVENT_TYPE type,void*data=0);
    void postEvent(EVENT_TYPE type,void*data=0);    
    void installEventFilter(QObject*obj);
    void removeEventFilter(QObject*obj);
    void addWidget(QWidget*w);
    void showWidget(QWidget*w);    
    static App*getInstance();
  };
  
}

#endif
