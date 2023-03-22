#ifndef MESSAGE_VIEW_H_
#define MESSAGE_VIEW_H_
#include"comp_search_bar.hpp"
#include"comp_content_list_view.hpp"
namespace qt_elevencent{
  class MsgView:public QWidget{
    Q_OBJECT
  public:
    explicit MsgView(QWidget*parent=nullptr);
  private:
    QVBoxLayout*m_vLayout;      
    CompContentListView*m_contentV;
    CompSearchBar*m_searchBar;
  };
}

#endif
