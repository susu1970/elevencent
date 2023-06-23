#include"setting_view.h"
#include"global.h"
#include"app.h"
#include<string>
#include<iostream>
using namespace qt_elevencent;
using namespace std;

static std::unordered_map<SETTING_BTN_IDX,QString>s_btnTextUM={
  {SETTING_BTN_IDX::SIGN_UP,"sign up"},
  {SETTING_BTN_IDX::LOGIN,"login in"},
  {SETTING_BTN_IDX::CMD,"cmd"},
};
#define BTN_IDX2STR(idx) (s_btnTextUM.find((SETTING_BTN_IDX)idx)==s_btnTextUM.end()?"":s_btnTextUM[(SETTING_BTN_IDX)idx])

SettingView::SettingView(QWidget*parent):QWidget{parent}{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  
  setContentsMargins(0,0,0,0);  
  m_vLayout=new QVBoxLayout(this);
  m_vLayout->setContentsMargins(0,0,0,0);
  m_vLayout->setSpacing(0);      
  setLayout(m_vLayout);
  m_listV=new CompContentListView(this);
  m_vLayout->addWidget(m_listV);  
  m_vLayout->setStretchFactor(m_listV,1);  
  connect(m_listV,SIGNAL(clicked(QModelIndex)),this,SLOT(onItemClicked(QModelIndex)));            
  for(int i=0;i<(int)SETTING_BTN_IDX::END;++i){
    m_listV->m_model->addItem(CompContentListItemData(BTN_IDX2STR(i)));
  }
  m_signupV=new SignUpView(this);
  connect(m_signupV->m_topV->m_back,&QPushButton::clicked,[this](){
    showContent(m_listV);
  });
  m_vLayout->addWidget(m_signupV);  
  m_vLayout->setStretchFactor(m_signupV,1);
  m_loginV=new LoginView(this);
  connect(m_loginV->m_topV->m_back,&QPushButton::clicked,[this](){
    showContent(m_listV);
  });
  m_vLayout->addWidget(m_loginV);  
  m_vLayout->setStretchFactor(m_loginV,1);      
  m_contents.push_back(m_listV);
  m_contents.push_back(m_signupV);
  m_contents.push_back(m_loginV);  
  showContent(m_listV);
}
void SettingView::showContent(QWidget*w){
  for(auto&c:m_contents)
    c->hide();
  w->show();
}
void SettingView::onItemClicked(QModelIndex index){
  switch(index.row()){
  case ((int)SETTING_BTN_IDX::CMD):
    App::getInstance()->sendEvent(EVENT_TYPE_CMD_VIEW_SHOW);
    break;
  case ((int)SETTING_BTN_IDX::SIGN_UP):
    showContent(m_signupV);
    break;
  case ((int)SETTING_BTN_IDX::LOGIN):
    showContent(m_loginV);
    break;
  default:
    break;
  }
}
