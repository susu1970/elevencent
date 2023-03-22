#ifndef APP_H_
#define APP_H_
#include<QMutex>
#include<QRect>
#include"home_view.h"
namespace qt_elevencent{
class App{
private:
  ~App();  
  App();
  App(const App&)=delete;
  const App&operator=(const App&)=delete;
  static App*ms_instance;
  static QMutex ms_instanceMutex;
private:
  void show();
public:
  HomeView*m_hv;
public:
  int run(QApplication*qapp);  
  static App*getInstance();
  
};

}

#endif
