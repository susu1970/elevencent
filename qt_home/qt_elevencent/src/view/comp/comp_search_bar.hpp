#ifndef _COMP_SEARCH_BAR_HPP_
#define _COMP_SEARCH_BAR_HPP_

#include<QWidget>
#include<QVBoxLayout>
#include<QLineEdit>
#include<QPushButton>

namespace qt_elevencent{
  class CompSearchBar:public QWidget{
    Q_OBJECT
  public:
    explicit CompSearchBar(QWidget*parent=nullptr):QWidget{parent}{
      using namespace std;
      setStyleSheet("background-color:#868686;");
      setContentsMargins(0,0,0,0);
      setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      m_hLayout=new QHBoxLayout(this);
      m_hLayout->setContentsMargins(0,0,0,0);
      setLayout(m_hLayout);
      m_hLayout->setAlignment(Qt::AlignCenter);
      m_hLayout->setSpacing(7);  
      m_searchEdit=new QLineEdit(this);
      m_searchEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
      m_searchEdit->setStyleSheet("background-color:#282a36;font: 20px;color:#839192;font-size: large;border-radius:4;border:1px solid black;font-family:Microsoft YaHei;");      
      m_hLayout->addWidget(m_searchEdit);
      m_hLayout->setStretchFactor(m_searchEdit,12);
      m_searchEdit->show();  
      m_searchBtn=new QPushButton("Q",this);
      m_searchBtn->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
      m_searchBtn->setStyleSheet("background-color:#282a36;font-weight:bold;font: bold 16px;color:gray;border-radius:8;");
      m_hLayout->addWidget(m_searchBtn);
      m_hLayout->setStretchFactor(m_searchBtn,1);
      m_searchBtn->hide();
    }
  public:
    QLineEdit*m_searchEdit;
    QPushButton*m_searchBtn;
    QHBoxLayout*m_hLayout;
  };  
}

#endif
