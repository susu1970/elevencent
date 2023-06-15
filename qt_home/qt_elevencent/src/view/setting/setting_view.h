#ifndef SETTING_VIEW_H_
#define SETTING_VIEW_H_

#include"comp_search_bar.hpp"
#include"sign_up_view.h"
#include"login.h"
#include"comp_content_list_view.hpp"
#include<list>
namespace qt_elevencent{
  enum class SETTING_BTN_IDX{
    SIGN_UP,
    LOGIN,
    AVATAR,
    NICK_NAME,
    CMD,

    END
  };
  class SettingView:public QWidget{
    Q_OBJECT
  public:
    explicit SettingView(QWidget*parent=nullptr);
  private:
    void showContent(QWidget*w);
  private:
    QVBoxLayout*m_vLayout;
    std::list<QWidget*>m_contents;
    SignUpView*m_signupV;
    LoginView*m_loginV;    
    CompContentListView*m_listV;
			       
  public slots:
    void onItemClicked(QModelIndex);
  };
  
}

#endif
