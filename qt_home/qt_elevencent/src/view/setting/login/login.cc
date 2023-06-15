#include<QString>
#include<QFontMetrics>
#include"app.h"
#include"rsa.hpp"
#include"toast_model.h"
#include"login.h"
#include"net_model.h"
#include"global.h"
using namespace std;
using namespace qt_elevencent;
LoginBtnView::LoginBtnView(QWidget*parent):QWidget{parent}{
  setContentsMargins(0,0,0,0);
  setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);  
  setStyleSheet("background-color:#282a36;border-radius:7;border: 1px solid black;");
  m_btn=new QPushButton("Go",this);
  m_btn->setContentsMargins(0,0,0,0);
  m_btn->setStyleSheet("background-color:#282a36;font: 30px;color:#a97bbe;border-radius:7;");
  m_btn->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  m_hLayout=new QHBoxLayout(this);
  m_hLayout->setContentsMargins(0,0,0,0);
  m_hLayout->setSpacing(0);    
  setLayout(m_hLayout);
  m_hLayout->addWidget(m_btn);
  m_hLayout->setStretchFactor(m_btn,10);
}
LoginView::LoginView(QWidget*parent):QWidget{parent}{
  setAttribute(Qt::WA_StyledBackground);
  setStyleSheet("background-color:#282a36;");
  setContentsMargins(0,0,0,0);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  
  m_vLayout=new QVBoxLayout(this);
  m_vLayout->setContentsMargins(0,0,0,0);  
  setLayout(m_vLayout);
  m_vLayout->setAlignment(Qt::AlignLeft);
  m_vLayout->setSpacing(25);
  m_topV=new CompTopTitleView(this);
  m_topV->m_label->setText("login");  
  m_vLayout->addWidget(m_topV);
  m_vLayout->setStretchFactor(m_topV,4);  
  m_vLayout->addSpacerItem(new QSpacerItem(0,SCREEN_H/10,QSizePolicy::Expanding,QSizePolicy::Expanding));
  m_user=new CompInputLine(this);
  m_user->setContentsMargins(5,0,38,0);
  m_user->m_label->setText("username");
  m_vLayout->addWidget(m_user);
  m_vLayout->setStretchFactor(m_user,3);
  m_passwd=new CompInputLine(this);
  m_passwd->setContentsMargins(10,0,38,0);  
  m_passwd->m_label->setText("password");
  m_vLayout->addWidget(m_passwd);
  m_vLayout->setStretchFactor(m_passwd,3);
  m_btn=new LoginBtnView(this);
  connect(m_btn->m_btn,&QPushButton::clicked,[this](){
    if(m_user->m_lineEdit->text().isEmpty()){
      m_user->m_lineEdit->setText("user name should not be empty");
      return;
    }
    NetModel::Server::Host curhost;
    App::getInstance()->sendEvent(EVENT_TYPE_GET_CONNED_SERVER,&curhost);    
    if(!curhost.isConn){
      ToastShowArg arg("server host not connected\n");
      App::getInstance()->sendEvent(EVENT_TYPE_TOAST_SHOW,&arg);      
      return;
    }
    if(curhost.rsaKeypub.str.empty()){
      ToastShowArg arg("server's rsa pubkey empty\n");
      App::getInstance()->sendEvent(EVENT_TYPE_TOAST_SHOW,&arg);
      return;
    }
    string crypwd=RSA::cryptWithRand16(m_passwd->m_lineEdit->text().toStdString(),curhost.rsaKeypub);
    if(crypwd.empty()){
      m_passwd->m_lineEdit->setText("passwd format error");
      return;
    }
    uint16_t nameLen=m_user->m_lineEdit->text().size();
    uint16_t crypwdLen=crypwd.size();
    int txlen=sizeof(TcpProtocol::Login)+nameLen+crypwdLen;
    char*txbuf=new char[txlen];
    TcpProtocol::Login*login=(TcpProtocol::Login*)txbuf;
    memset(login,0,sizeof(TcpProtocol::Login));
    login->type=TcpProtocol::Head::TYPE::LOGIN;
    login->nameLen=nameLen;
    login->crypwdLen=crypwdLen;
    char*name=(char*)(login+1);
    memcpy(name,m_user->m_lineEdit->text().toLatin1().data(),nameLen);
    char*pwd=name+nameLen;
    memcpy(pwd,crypwd.c_str(),crypwdLen);
    login->h2n();
    NetModel::Tx tx(NetModel::STATE_OUT::LOGIN,new NetModel::StateCtx(txbuf,txlen,[](NetModel::StateCtx*ctx){
      delete[]((char*)ctx->m_data);
    }));
    App::getInstance()->sendEvent(EVENT_TYPE_NET_TX,&tx);
  });
  m_btn->setContentsMargins(38,0,38,0);
  m_vLayout->addWidget(m_btn);
  m_vLayout->setStretchFactor(m_btn,6);
  m_vLayout->addStretch(20);
}
