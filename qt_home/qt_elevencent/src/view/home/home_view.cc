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
  setStyleSheet("background-color:black;");
  setContentsMargins(0,0,0,0);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  
  m_vLayout=new QVBoxLayout(this);
  m_vLayout->setContentsMargins(0,0,0,0);
  m_vLayout->setSpacing(0);
  setLayout(m_vLayout);  
  m_hcv=new HomeContentView(this);
  m_hcv->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_vLayout->addWidget(m_hcv);
  m_vLayout->setStretchFactor(m_hcv,13);
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
  setContentsMargins(0,0,0,0);
  setLayout(m_hLayout);
  m_hLayout->setAlignment(Qt::AlignCenter);
  m_hLayout->setSpacing(0);
  m_hLayout->setContentsMargins(0,0,0,0);
  m_msgV=new MsgView(this);
  m_hLayout->addWidget(m_msgV);
  m_hLayout->setStretchFactor(m_msgV,1);
  m_resourceV=new ResourceView(this);
  m_hLayout->addWidget(m_resourceV);
  m_hLayout->setStretchFactor(m_resourceV,1);
  m_settingV=new SettingView(this);
  m_hLayout->addWidget(m_settingV);
  m_hLayout->setStretchFactor(m_settingV,1);
  m_contentUM[HOME_BOTTOM_IDX::MESSAGE]=(QWidget*)m_msgV;
  m_contentUM[HOME_BOTTOM_IDX::RESOURCE]=(QWidget*)m_resourceV;
  m_contentUM[HOME_BOTTOM_IDX::SETTING]=(QWidget*)m_settingV;
}
HomeBottomView::HomeBottomView(QWidget*parent):QWidget{parent}{
  setStyleSheet("background-color:#282a36;");
  setContentsMargins(0,0,0,0);  
  m_btnGroup=new QButtonGroup(this);
  m_btnGroup->setExclusive(true);
  connect(m_btnGroup,SIGNAL(idClicked(int)),this,SLOT(onBtnGroupClicked(int)));
  m_hLayout=new QHBoxLayout(this);
  m_hLayout->setContentsMargins(0,1,0,0);
  setLayout(m_hLayout);
  m_hLayout->setAlignment(Qt::AlignCenter);
  m_hLayout->setSpacing(2);
  for(int i=0;i<(int)HOME_BOTTOM_IDX::END;++i){
    QPushButton*btn=new QPushButton(BTN_IDX2STR(i),this);
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    btn->setContentsMargins(0,0,0,0);
    m_hLayout->addWidget(btn);
    m_hLayout->setStretchFactor(btn,1);    
    btn->setStyleSheet("background-color:#282a36;font:  20px;color:gray;border-radius:1;font-family:Microsoft YaHei;");
    m_btnGroup->addButton(btn,i);
    btn->show();
  }
}
void HomeBottomView::paintEvent(QPaintEvent*ev){
  QPainter p(this);
  p.setPen(QColor("black")); 
  p.drawRect(20,0,width()-1-20*2,height()-1);
}
void HomeView::onBottomClicked(int idx){
  m_hcv->showByIdx((HOME_BOTTOM_IDX)idx);
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
  for(int i=0;i<(int)HOME_BOTTOM_IDX::END;++i){
    if(i==(int)idx)
      m_btnGroup->button(i)->setStyleSheet("background-color:#282a36;font: 20px;color:#4e3b6a;border-radius:1;font-family:Microsoft YaHei;");
    else
      m_btnGroup->button(i)->setStyleSheet("background-color:#282a36;font: 20px;color:gray;border-radius:1;font-family:Microsoft YaHei;");
  }

  ((HomeView*)parentWidget())->onBottomClicked(idx);
}
