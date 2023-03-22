#ifndef _RESOURCE_VIEW_H_
#define _RESOURCE_VIEW_H_

#include"comp_search_bar.hpp"
#include"comp_content_list_view.hpp"

namespace qt_elevencent{
  class ResourceView:public QWidget{
    Q_OBJECT
  public:
    explicit ResourceView(QWidget*parent=nullptr);
  private:
    
  private:
    QVBoxLayout*m_vLayout;      
    CompContentListView*m_contentV;
    CompSearchBar*m_searchBar;
  };
}

#endif
