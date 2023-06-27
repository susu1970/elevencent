#ifndef _COMP_INPUT_LINE_HPP_
#define _COMP_INPUT_LINE_HPP_

#include<QWidget>
#include<QVBoxLayout>
#include<QLineEdit>
#include<QPushButton>
#include<QLabel>

namespace qt_elevencent{
  #define COMP_INPUTLINE_MAX_LENGTH 1024
  class CompInputLine:public QWidget{
    Q_OBJECT
  public:
    explicit CompInputLine(QWidget*parent=nullptr):QWidget{parent}{
      using namespace std;
      setStyleSheet("background-color:#868686;");
      setContentsMargins(0,0,0,0);
      setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
      m_hLayout=new QHBoxLayout(this);
      m_hLayout->setContentsMargins(0,0,0,0);
      setLayout(m_hLayout);
      m_hLayout->setAlignment(Qt::AlignLeft);
      //      m_hLayout->setSpacing(10);
      m_hLayout->setSpacing(0);        
      m_label=new QLabel(this);
      m_label->setContentsMargins(0,0,0,0);
      m_label->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
      m_label->setStyleSheet("background-color:#282a36;font: 22px;color:gray;font-size: large;border-radius:4;font-family:Microsoft YaHei;");
      m_hLayout->addWidget(m_label);
      m_hLayout->setStretchFactor(m_label,0);
      m_hLayout->addSpacerItem(new QSpacerItem(10,0,QSizePolicy::Expanding,QSizePolicy::Expanding));      
      m_lineEdit=new QLineEdit(this);
      m_lineEdit->setMaxLength(COMP_INPUTLINE_MAX_LENGTH);
      m_lineEdit->setContentsMargins(0,0,0,0);  
      m_lineEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
      m_lineEdit->setStyleSheet("background-color:#282a36;font:  20px;color:#839192;font-size: large;border-radius:4;border:1px solid black;font-family:Microsoft YaHei;");
      m_hLayout->addWidget(m_lineEdit);
      m_hLayout->setStretchFactor(m_lineEdit,1);
      m_hLayout->addSpacerItem(new QSpacerItem(2,0,QSizePolicy::Expanding,QSizePolicy::Expanding));            
      m_btn=new QPushButton(this);
      m_btn->setContentsMargins(0,0,0,0);  
      m_btn->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
      m_btn->setStyleSheet("background-color:#282a36;font: 22px;color:#839192;border-radius:4;border:1px solid black;");
      m_hLayout->addWidget(m_btn);
      m_hLayout->setStretchFactor(m_btn,0);
      m_btn->hide();
    }
  public:
    QLineEdit*m_lineEdit;
    QLabel*m_label;
    QPushButton*m_btn;
    QHBoxLayout*m_hLayout;
  };  
}

#endif
