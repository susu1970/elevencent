#include"message_view.h"
#include"global.h"
#include"app.h"

#include<string>
#include<iostream>
using namespace qt_elevencent;
using namespace std;
SendUserMsgView::SendUserMsgView(QWidget*parent):QWidget{parent}{
  setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);  
  setStyleSheet("background-color:black;");
  setContentsMargins(0,0,0,0);
  m_vLayout=new QVBoxLayout(this);
  m_vLayout->setSpacing(2);
  m_vLayout->setContentsMargins(3,0,3,2);
  setLayout(m_vLayout);
  m_vLayout->setAlignment(Qt::AlignLeft);
  m_sendMsg=new CompInputLine(this);
  m_vLayout->addWidget(m_sendMsg);
  m_sendMsg->m_label->setText("msg");
  m_toUser=new CompInputLine(this);
  m_vLayout->addWidget(m_toUser);  
  m_toUser->m_label->setText("user");
  m_toUser->m_btn->setText("send");
  m_toUser->m_btn->show();
  connect(m_toUser->m_btn,SIGNAL(clicked()),this,SLOT(onUserDoSendMsg()));
}
void SendUserMsgView::onUserDoSendMsg(){
  QString user=m_toUser->m_lineEdit->text();
  user.replace("\\","\\\\");
  user.replace("\"","\\\"");
  QString msg=m_sendMsg->m_lineEdit->text();
  msg.replace("\\","\\\\");
  msg.replace("\"","\\\"");    
  QString cmd=QString("net user_resource sendmsg -n \"")+user+"\" -m \""+msg+"\"";
  qDebug()<<"cmd: "<<cmd;
  CmdLineModel cmdModel(cmd);
  App::getInstance()->sendEvent(EVENT_TYPE_CMD_LINE,&cmdModel);
}
void SendUserMsgView::paintEvent(QPaintEvent*ev){
  QPainter p(this);
  p.setPen(QColor("black"));
  p.drawRect(7,height()-1,width()-2*7,0);
}
MsgView::MsgView(QWidget*parent):QWidget{parent}{
  setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);  
  setStyleSheet("background-color:black;");
  setContentsMargins(0,0,0,0);  
  m_vLayout=new QVBoxLayout(this);
  m_vLayout->setSpacing(0);
  m_vLayout->setContentsMargins(0,0,0,0);
  setLayout(m_vLayout);
  m_searchBar=new CompSearchBar(this);
  m_searchBar->setContentsMargins(2,2,2,2);
  m_searchBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  
  m_vLayout->addWidget(m_searchBar);
  m_vLayout->setStretchFactor(m_searchBar,1);        
  m_searchBar->show();
  m_sendMsgV=new SendUserMsgView(this);
  m_vLayout->addWidget(m_sendMsgV);
  m_vLayout->setStretchFactor(m_sendMsgV,2);
  m_sendMsgV->show();
  m_contentV=new CompContentListView(this);
  m_vLayout->addWidget(m_contentV);
  m_vLayout->setStretchFactor(m_contentV,12);
  m_contentV->show();

  connect(m_searchBar->m_searchBtn,SIGNAL(clicked()),this,SLOT(onSearchBarDoSearch()));
  connect(m_searchBar->m_searchEdit,SIGNAL(returnPressed()),this,SLOT(onSearchBarDoSearch()));
}
void MsgView::onSearchBarDoSearch(){
}
