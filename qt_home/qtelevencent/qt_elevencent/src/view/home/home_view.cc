#include "home_view.h"
#include<iostream>
#include"global.h"
#include<QDebug>

using namespace std;
using namespace qt_elevencent;

static std::unordered_map<HOME_BOTTOM_IDX,QString>s_btnTextUM={
  {HOME_BOTTOM_IDX::MESSAGE,"message"},
  {HOME_BOTTOM_IDX::RESOURCE,"resource"},
  {HOME_BOTTOM_IDX::SETTING,"setting"},
};
#define BTN_IDX2STR(idx) (s_btnTextUM.find((HOME_BOTTOM_IDX)idx)==s_btnTextUM.end()?"":s_btnTextUM[(HOME_BOTTOM_IDX)idx])

HomeView::HomeView(QWidget*parent):QWidget{parent}{
  setGeometry(0,0,SCREEN_W/2,(SCREEN_H/3)*2);
  setStyleSheet("background-color:black;");
  m_vLayout=new QVBoxLayout(this);
  setLayout(m_vLayout);  
  m_hcv=new HomeContentView(this);
  m_hcv->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_vLayout->addWidget(m_hcv);
  m_vLayout->setStretchFactor(m_hcv,9);
  m_hbv=new HomeBottomView(this);
  m_hbv->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_vLayout->addWidget(m_hbv);
  m_vLayout->setStretchFactor(m_hbv,1);
  m_hcv->show();
  m_hcv->hideByIdx();      
  m_hbv->show();
}
HomeContentView::HomeContentView(QWidget*parent):QWidget{parent}{
  setStyleSheet("background-color:black;");
  m_hLayout=new QHBoxLayout(this);
  setLayout(m_hLayout);
  m_hLayout->setAlignment(Qt::AlignCenter);
  m_hLayout->setSpacing(0);    
  m_msgV=new MsgView(this);
  m_msgV->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  m_hLayout->addWidget(m_msgV);
  m_hLayout->setStretchFactor(m_msgV,1);
  m_resourceV=new ResourceView(this);
  m_resourceV->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);    
  m_hLayout->addWidget(m_resourceV);
  m_hLayout->setStretchFactor(m_resourceV,1);
  m_settingV=new SettingView(this);
  m_settingV->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);    
  m_hLayout->addWidget(m_settingV);
  m_hLayout->setStretchFactor(m_settingV,1);
  m_contentUM[HOME_BOTTOM_IDX::MESSAGE]=(QWidget*)m_msgV;
  m_contentUM[HOME_BOTTOM_IDX::RESOURCE]=(QWidget*)m_resourceV;
  m_contentUM[HOME_BOTTOM_IDX::SETTING]=(QWidget*)m_settingV;
}
HomeBottomView::HomeBottomView(QWidget*parent):QWidget{parent}{
  setStyleSheet("background-color:black;");
  m_btnGroup=new QButtonGroup(this);
  m_btnGroup->setExclusive(true);
  connect(m_btnGroup,SIGNAL(idClicked(int)),this,SLOT(onBtnGroupClicked(int)));
  m_hLayout=new QHBoxLayout(this);
  setLayout(m_hLayout);
  m_hLayout->setAlignment(Qt::AlignCenter);
  m_hLayout->setSpacing(4);
  for(int i=0;i<(int)HOME_BOTTOM_IDX::END;++i){
    QPushButton*btn=new QPushButton(BTN_IDX2STR(i),this);
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_hLayout->addWidget(btn);
    m_hLayout->setStretchFactor(btn,1);    
    btn->setStyleSheet("background-color:#282a36;font-weight:bold;font: bold 16px;color:gray;border-radius:5;");    
    m_btnGroup->addButton(btn,i);
    btn->show();
  }
}
void HomeView::onBottomClicked(int idx){
  return m_hcv->showByIdx((HOME_BOTTOM_IDX)idx);
}
void HomeContentView::showByIdx(HOME_BOTTOM_IDX idx){
  if(m_contentUM.find(idx)!=m_contentUM.end()){
    hideByIdx();
    m_contentUM[idx]->show();    
  }
}
void HomeContentView::hideByIdx(HOME_BOTTOM_IDX idx){
  switch(idx){
  case HOME_BOTTOM_IDX::END:{
    for(int i=0;i<(int)HOME_BOTTOM_IDX::END;++i)
      hideByIdx((HOME_BOTTOM_IDX)i);
  }
    break;
  default:
    if(m_contentUM.find(idx)!=m_contentUM.end())
      m_contentUM[idx]->hide();
    break;
  }
}
void HomeBottomView::onBtnGroupClicked(int idx){
  if(!parentWidget())
    return;
  ((HomeView*)parentWidget())->onBottomClicked(idx);
}
