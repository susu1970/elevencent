#ifndef _LOGIN_VIEW_H_
#define _LOGIN_VIEW_H_

#include"comp_input_line.hpp"
#include"comp_top_title_view.hpp"
#include<QSpacerItem>
#include<QHBoxLayout>
#include<QLabel>
namespace qt_elevencent{
  class LoginBtnView:public QWidget{
    Q_OBJECT
  public:
    explicit LoginBtnView(QWidget*parent=nullptr);
  public:
    QHBoxLayout*m_hLayout;
    QPushButton*m_btn;
  };
  class LoginView:public QWidget{
    Q_OBJECT
  public:
    explicit LoginView(QWidget*parent=nullptr);
  public:
    
  public:
    QVBoxLayout*m_vLayout;
    CompTopTitleView*m_topV;
    CompInputLine*m_user,*m_passwd;
    LoginBtnView*m_btn;
  };
}

#endif
