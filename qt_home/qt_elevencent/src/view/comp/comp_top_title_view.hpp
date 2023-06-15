#ifndef _COMP_TOP_TITLE_VIEW_HPP_
#define _COMP_TOP_TITLE_VIEW_HPP_
#include<QSpacerItem>
#include<QHBoxLayout>
#include<QLabel>
#include<QWidget>
#include<QtGui>

namespace qt_elevencent{

  class CompTopTitleView:public QWidget{
    Q_OBJECT
  public:
    explicit CompTopTitleView(QWidget*parent=nullptr):QWidget(parent){
      setAttribute(Qt::WA_StyledBackground);      
      setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);    
      setContentsMargins(0,0,0,0);
      setStyleSheet("background-color:#282a36;");
      m_hLayout=new QHBoxLayout(this);
      m_hLayout->setContentsMargins(0,0,0,0);
      m_hLayout->setSpacing(25);
      setLayout(m_hLayout);
      m_back=new QPushButton("<",this);
      m_back->setContentsMargins(15,15,0,15);
      m_back->setStyleSheet("background:transparent;font: 31px;color:#a97bbe;border-radius:4;border:1px solid black;");
      m_back->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
      m_label=new QLabel(this);
      m_label->setContentsMargins(0,0,0,0);
      m_label->setAlignment(Qt::AlignCenter);    
      m_label->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
      m_label->setStyleSheet("background:transparent;font: 26px;color:gray;font-size: large;border-radius:4;border: none;");      
      m_hLayout->addWidget(m_back);
      m_hLayout->setStretchFactor(m_back,1);
      m_hLayout->addWidget(m_label);
      m_hLayout->setStretchFactor(m_label,4);
      m_hLayout->addStretch(2);
    }
    QLabel*m_label;
    QPushButton*m_back;
    QHBoxLayout*m_hLayout;
  };
}

#endif
