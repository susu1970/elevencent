#include"resource_view.h"
#include"global.h"
#include<string>
#include<iostream>
using namespace qt_elevencent;
using namespace std;
ResourceView::ResourceView(QWidget*parent):QWidget{parent}{
  setStyleSheet("background-color:black;");
  m_vLayout=new QVBoxLayout(this);
  m_vLayout->setSpacing(0);      
  setLayout(m_vLayout);
  m_searchBar=new CompSearchBar(this);
  m_searchBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  
  m_vLayout->addWidget(m_searchBar);
  m_vLayout->setStretchFactor(m_searchBar,1);        
  m_searchBar->show();  
  m_contentV=new CompContentListView(this);
  m_vLayout->addWidget(m_contentV);
  m_vLayout->setStretchFactor(m_contentV,10);
  m_contentV->show();
}
