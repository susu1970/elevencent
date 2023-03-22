#ifndef _COMP_INPUT_LINE_HPP_
#define _COMP_INPUT_LINE_HPP_

#include<QWidget>
#include<QVBoxLayout>
#include<QLineEdit>
#include<QPushButton>
#include<QLabel>

namespace qt_elevencent{
  class CompInputLine:public QWidget{
    Q_OBJECT
  public:
    explicit CompInputLine(QWidget*parent=nullptr):QWidget{parent}{
      using namespace std;
      setStyleSheet("background-color:#868686;");
      m_hLayout=new QHBoxLayout(this);
      setLayout(m_hLayout);
      m_hLayout->setAlignment(Qt::AlignLeft);
      m_hLayout->setSpacing(5);  

      m_label=new QLabel(this);
      m_label->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
      m_label->setStyleSheet("background-color:#282a36;font-weight:bold;font: bold 15px;color:gray;border-radius:10;");
      m_hLayout->addWidget(m_label);
      m_hLayout->setStretchFactor(m_label,12);
      m_label->show();

      m_lineEdit=new QLineEdit(this);
      m_lineEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
      m_lineEdit->setStyleSheet("background-color:#282a36;font-weight:bold;font: bold 15px;color:gray;border-radius:10;");
      m_hLayout->addWidget(m_lineEdit);
      m_hLayout->setStretchFactor(m_lineEdit,12);
      m_lineEdit->show();  
    }
  public:
    QLineEdit*m_lineEdit;
    QLabel*m_label;
    QHBoxLayout*m_hLayout;
  };  
}

#endif
