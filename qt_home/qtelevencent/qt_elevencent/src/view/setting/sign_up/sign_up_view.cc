#include"sign_up_view.h"
using namespace std;
using namespace qt_elevencent;

SignUpView::SignUpView(QWidget*parent):QWidget{parent}{
  setStyleSheet("background-color:#868686;");
  m_vLayout=new QVBoxLayout(this);
  setLayout(m_vLayout);
  m_vLayout->setAlignment(Qt::AlignLeft);
  m_vLayout->setSpacing(5);    
  
  m_userName=new CompInputLine(this);
  m_userName->m_label->setText("user name:");
  m_userName->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  m_vLayout->addWidget(m_userName);
  m_vLayout->setStretchFactor(m_userName,1);
  m_passwd=new CompInputLine(this);
  m_passwd->m_label->setText("password:");
  m_passwd->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  m_vLayout->addWidget(m_passwd);
  m_vLayout->setStretchFactor(m_passwd,1);

  m_userName->show();
  m_passwd->show();  
}
