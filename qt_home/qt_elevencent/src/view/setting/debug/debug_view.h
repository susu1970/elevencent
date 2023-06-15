#ifndef DEBUG_VIEW_H_
#define DEBUG_VIEW_H_

#include"comp_input_line.hpp"
#include"comp_search_bar.hpp"
#include"comp_content_list_view.hpp"

namespace qt_elevencent{
  class DebugView:public QWidget{
    Q_OBJECT
  public:
    explicit DebugView(QWidget*parent=0);
  private:
    QVBoxLayout*m_vLayout;          
  public:
    CompSearchBar*m_searchBar;
    
    CompContentListView*m_contentListV;
  public slots:
    void onSearchBarDoSearch();
  };
}

#endif
