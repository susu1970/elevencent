#ifndef SETTING_VIEW_H_
#define SETTING_VIEW_H_

#include"comp_search_bar.hpp"
#include"comp_content_list_view.hpp"

namespace qt_elevencent{
  enum class SETTING_BTN_IDX{
    SIGN_UP,
    LOGIN_IN,
    AVATAR,
    NICK_NAME,    
    END
  };
  class SettingView:public QWidget{
    Q_OBJECT
  public:
    explicit SettingView(QWidget*parent=nullptr);
  private:
  private:
    QVBoxLayout*m_vLayout;      
    CompContentListView*m_contentV;
  public slots:
    void onItemClicked(QModelIndex);
  };
  

}

#endif
