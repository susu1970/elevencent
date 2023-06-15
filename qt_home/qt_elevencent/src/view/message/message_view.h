#ifndef MESSAGE_VIEW_H_
#define MESSAGE_VIEW_H_
#include"comp_search_bar.hpp"
#include"comp_content_list_view.hpp"
#include"comp_input_line.hpp"
#include"msg_model.h"
#include<QCommandLineParser>
#include<QFontMetrics>
namespace qt_elevencent{
  class SendUserMsgView:public QWidget{
    Q_OBJECT
  public:
    explicit SendUserMsgView(QWidget*parent=nullptr);
    CompInputLine*m_sendMsg,*m_toUser;
    QVBoxLayout*m_vLayout;
    void paintEvent(QPaintEvent*ev);
  public slots:
    void onUserDoSendMsg();
  };
  class MsgView:public QWidget{
    Q_OBJECT
  public:
    explicit MsgView(QWidget*parent=nullptr);
  private:
    QVBoxLayout*m_vLayout;      
    CompContentListView*m_contentV;
    SendUserMsgView*m_sendMsgV;
    CompSearchBar*m_searchBar;
  public:
    MsgModel m_msgM;
  public slots:
    void onSearchBarDoSearch();
  };
}

#endif
