#ifndef MESSAGE_VIEW_H_
#define MESSAGE_VIEW_H_
#include"comp_search_bar.hpp"
#include"comp_content_list_view.hpp"
#include"comp_input_line.hpp"
#include"comp_top_title_view.hpp"

#include<QCommandLineParser>
#include<QFontMetrics>
namespace qt_elevencent{
  class MsgContentItemPriv{
  public:
    
  };
  class MsgContentListDelegate:public CompContentListDelegate{
    Q_OBJECT
  public:
    MsgContentListDelegate(QObject*parent=0):CompContentListDelegate(parent){}
    virtual void paint(QPainter*painter,const QStyleOptionViewItem&option,const QModelIndex&index)const override;
    virtual QSize sizeHint(const QStyleOptionViewItem&option,const QModelIndex&index)const override;
    virtual ~MsgContentListDelegate(){}
  };
  class MsgContentView:public QWidget{
    Q_OBJECT
  public:
    explicit MsgContentView(QWidget*parent=nullptr);
    CompTopTitleView*m_topV;
    QVBoxLayout*m_vLayout;
    CompContentListView*m_listV;
    CompInputLine*m_sendMsg;
  };
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
  public:
    QVBoxLayout*m_vLayout;      
    CompContentListView*m_contentV;
    SendUserMsgView*m_sendMsgV;
    CompSearchBar*m_searchBar;
  public slots:
    void onSearchBarDoSearch();
  };
}

#endif
