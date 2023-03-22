#ifndef _SIGN_UP_VIEW_H_
#define _SIGN_UP_VIEW_H_

#include"comp_input_line.hpp"

namespace qt_elevencent{
  class SignUpView:public QWidget{
    Q_OBJECT
  public:
    explicit SignUpView(QWidget*parent=nullptr);
  private:
    
  private:
    QVBoxLayout*m_vLayout;      
    CompInputLine*m_userName,*m_passwd;
    QPushButton*m_signUpBtn;
  };
}

#endif
