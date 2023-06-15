#ifndef _SIGN_UP_VIEW_H_
#define _SIGN_UP_VIEW_H_

#include"comp_input_line.hpp"
#include"comp_top_title_view.hpp"
#include<QSpacerItem>
#include<QHBoxLayout>
#include<QLabel>
namespace qt_elevencent{
  class SignUpBtnView:public QWidget{
    Q_OBJECT
  public:
    explicit SignUpBtnView(QWidget*parent=nullptr);
  public:
    QHBoxLayout*m_hLayout;
    QPushButton*m_btn;

  };
  class SignUpView:public QWidget{
    Q_OBJECT
  public:
    explicit SignUpView(QWidget*parent=nullptr);
  public:
    
  public:
    QVBoxLayout*m_vLayout;
    CompTopTitleView*m_topV;
    CompInputLine*m_user,*m_passwd;
    SignUpBtnView*m_btn;
  };
}

#endif
