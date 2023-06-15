#ifndef APP_VIEW_HPP_
#define APP_VIEW_HPP_
#include"global.h"
#include<QWidget>
#include<QHBoxLayout>
#include<QVBoxLayout>

namespace qt_elevencent{
  class AppView:public QWidget{
    Q_OBJECT
  public:
    QVBoxLayout*m_vLayout;
  public:
    explicit AppView(QWidget*parent=0):QWidget(parent){
      setGeometry(SCREEN_W/3,SCREEN_H/5,SCREEN_W/3,(SCREEN_H/3)*2);
      setStyleSheet("background-color:#282a36;");
      setContentsMargins(0,0,0,0);
      m_vLayout=new QVBoxLayout(this);
      m_vLayout->setSpacing(0);
      m_vLayout->setContentsMargins(0,0,0,0);
      setLayout(m_vLayout);        
    }
  };
}

#endif
