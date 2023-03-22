#include"setting_view.h"
#include"global.h"
#include<string>
#include<iostream>
using namespace qt_elevencent;
using namespace std;
static std::unordered_map<SETTING_BTN_IDX,QString>s_btnTextUM={
  {SETTING_BTN_IDX::SIGN_UP,"sign up"},
  {SETTING_BTN_IDX::LOGIN_IN,"login in"},
  {SETTING_BTN_IDX::AVATAR,"avatar"},
  {SETTING_BTN_IDX::NICK_NAME,"nick name"},  
};
#define BTN_IDX2STR(idx) (s_btnTextUM.find((SETTING_BTN_IDX)idx)==s_btnTextUM.end()?"":s_btnTextUM[(SETTING_BTN_IDX)idx])

SettingView::SettingView(QWidget*parent):QWidget{parent}{
  setStyleSheet("background-color:black;");
  m_vLayout=new QVBoxLayout(this);
  m_vLayout->setSpacing(0);      
  setLayout(m_vLayout);
  m_contentV=new CompContentListView(this);
  connect(m_contentV,SIGNAL(clicked(QModelIndex)),this,SLOT(onItemClicked(QModelIndex)));            
  m_vLayout->addWidget(m_contentV);
  m_vLayout->setStretchFactor(m_contentV,1);
  m_contentV->show();
  for(int i=0;i<(int)SETTING_BTN_IDX::END;++i){
    m_contentV->addItem(new CompContentListItemData(BTN_IDX2STR(i)));
  }
  
}
void SettingView::onItemClicked(QModelIndex index){
  qDebug()<<"index.row: "<<index.row();  
}
