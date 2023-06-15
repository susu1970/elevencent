#include"debug_view.h"
#include"message_view.h"
#include"global.h"
#include<string>
#include<string>
#include<iostream>
#include<QCommandLineParser>
using namespace qt_elevencent;
using namespace std;

DebugView::DebugView(QWidget*parent):QWidget{parent}{
  setStyleSheet("background-color:black;");
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);    
  m_vLayout=new QVBoxLayout(this);
  m_vLayout->setSpacing(0);      
  setLayout(m_vLayout);
  m_searchBar=new CompSearchBar(this);
  m_searchBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  
  m_vLayout->addWidget(m_searchBar);
  m_vLayout->setStretchFactor(m_searchBar,1);        
  m_searchBar->show();  
  m_contentListV=new CompContentListView(this);
  m_vLayout->addWidget(m_contentListV);
  m_vLayout->setStretchFactor(m_contentListV,10);
  m_contentListV->show();

  connect(m_searchBar->m_searchBtn,SIGNAL(clicked()),this,SLOT(onSearchBarDoSearch()));
  connect(m_searchBar->m_searchEdit,SIGNAL(returnPressed()),this,SLOT(onSearchBarDoSearch()));
  
}
void DebugView::onSearchBarDoSearch(){}
