#ifndef TOAST_MODEL_H_
#define TOAST_MODEL_H_

#include<string>
#include<QString>
namespace qt_elevencent{
  class ToastShowArg{
  public:
    QString m_text;
    int m_time;
    ToastShowArg(QString&&text,int time=4000):m_text(std::forward<QString>(text)),m_time(time){}
    ToastShowArg(QString&text,int time=4000):m_text(std::forward<QString>(text)),m_time(time){}    
  };
  class ToastModel{
  public:
    
  };
}

#endif
