#ifndef CMD_VIEW_H_
#define CMD_VIEW_H_

#include"comp_input_line.hpp"
#include"comp_search_bar.hpp"
#include"comp_content_scroll.hpp"
#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QtGui>
#include<initializer_list>
#include<cmd_model.h>

namespace qt_elevencent{
  class CmdView:public QWidget{
    Q_OBJECT
  public:
    explicit CmdView(QWidget*parent=0);
    void output(CMD_VIEW_CONTENT_IDX target,QString&&text,bool append=true,QString&&preFormat="",QString&&tailFormat="");
    void output(CmdViewOutputArg&&arg);
    void paintEvent(QPaintEvent*ev);    
  private:
    QVBoxLayout*m_vLayout;          
  public:
    CompSearchBar*m_searchBar;
    CompContentScroll*m_hintScroll,*m_contentScroll;
  public slots:
    void onSearchBarDoSearch();
  };
}

#endif
