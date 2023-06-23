#include"message_view.h"
#include"resource.h"
#include"global.h"
#include"app.h"

#include<string>
#include<iostream>
using namespace qt_elevencent;
using namespace std;
extern QString get_wrap_word(const QFontMetrics&font,const QString&text,int limitSize);
void MsgContentListDelegate::paint(QPainter*painter,const QStyleOptionViewItem&option,const QModelIndex&index)const {
  if(!index.data(Qt::DisplayRole).canConvert<CompContentListItemData>())
    return QStyledItemDelegate::paint(painter,option,index);      
  const QRect&rect=option.rect;
  qDebug()<<"rect:"<<rect;
  auto itemData=(index.data(Qt::DisplayRole).value<CompContentListItemData>());
  MsgModel::MsgUnit*msgUnit=(MsgModel::MsgUnit*)itemData.m_priv;
  int lrgap=rect.width()/4;
  int gap=8;
  int msgWidth=rect.width()-lrgap-gap;
  QFont font;
  font.setFamily("Microsoft YaHei");
  font.setPixelSize(22);
  QFontMetrics fontM(font);
  QString wrapContent=get_wrap_word(fontM,QString::fromStdString(msgUnit->content),msgWidth);
  QSize wrapSize=fontM.size(Qt::TextExpandTabs,wrapContent);
  QRect msgRect;
  if(msgUnit->umask&RESOURCE_MASK_USER_POST_SENDER){
    msgRect=QRect(rect.x()+(rect.width()-gap-wrapSize.width()),rect.y(),wrapSize.width(),wrapSize.height());
  }else{
    msgRect=QRect(rect.x()+gap,rect.y(),wrapSize.width(),wrapSize.height());    
  }
  {//back-ground
    QColor backcolor("#2c2c2c");      
    painter->save();
    painter->fillRect(msgRect,backcolor);
    painter->restore();
  }    
  if(msgUnit->umask&RESOURCE_MASK_USER_POST_SENDER){
    //text
    {
      painter->save();
      painter->setPen(QColor("gray"));              
      painter->setFont(font);
      painter->drawText(msgRect,Qt::AlignLeft,wrapContent);      
      painter->restore();
    }
  }else{
    //text
    {
      painter->save();
      painter->setPen(QColor("#839192"));            
      painter->setFont(font);
      painter->drawText(msgRect,Qt::AlignLeft,wrapContent);
      painter->restore();    
    }    
  }
}
QSize MsgContentListDelegate::sizeHint(const QStyleOptionViewItem&option,const QModelIndex&index)const{
  if(!index.data(Qt::DisplayRole).canConvert<CompContentListItemData>())
    return QSize(0,0);
  const QRect&rect=option.rect;
  qDebug()<<"sizeHint, rect: "<<rect;
  auto itemData=(index.data(Qt::DisplayRole).value<CompContentListItemData>());
  MsgModel::MsgUnit*msgUnit=(MsgModel::MsgUnit*)itemData.m_priv;
  int lrgap=rect.width()/4;
  int gap=8;
  int msgWidth=rect.width()-lrgap-gap;
  QFont font;
  font.setFamily("Microsoft YaHei");
  font.setPixelSize(22);
  QFontMetrics fontM(font);
  QString wrapContent=get_wrap_word(fontM,QString::fromStdString(msgUnit->content),msgWidth);
  QSize wrapSize=fontM.size(Qt::TextExpandTabs,wrapContent);
  return QSize(rect.width(),wrapSize.height());
}
MsgContentView::MsgContentView(QWidget*parent):QWidget{parent}{
  setStyleSheet("background-color:#282a36;");
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setContentsMargins(0,0,0,0);  
  m_vLayout=new QVBoxLayout(this);
  m_vLayout->setContentsMargins(0,0,0,0);
  m_vLayout->setAlignment(Qt::AlignLeft);
  m_vLayout->setSpacing(1);  
  setLayout(m_vLayout);
  m_topV=new CompTopTitleView(this);
  m_vLayout->addWidget(m_topV);
  m_vLayout->setStretchFactor(m_topV,2);
  m_listV=new CompContentListView(this);
  m_listV->setSpacing(10);
  delete m_listV->m_delegate;
  m_listV->m_delegate=new MsgContentListDelegate(m_listV);
  m_listV->setItemDelegate(m_listV->m_delegate);
  m_vLayout->addWidget(m_listV);
  m_vLayout->setStretchFactor(m_listV,24);
  m_listV->show();
  m_sendMsg=new CompInputLine(this);
  m_vLayout->addWidget(m_sendMsg);
  m_sendMsg->m_btn->setText("send");
  m_sendMsg->m_btn->show();
  m_sendMsg->setContentsMargins(0,0,7,7);
  m_sendMsg->m_label->hide();
  m_vLayout->setStretchFactor(m_sendMsg,2);  
}
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
  
  connect(m_searchBar->m_searchBtn,SIGNAL(clicked()),this,SLOT(onSearchBarDoSearch()));
  connect(m_searchBar->m_searchEdit,SIGNAL(returnPressed()),this,SLOT(onSearchBarDoSearch()));

}
void MsgView::onSearchBarDoSearch(){
}
