#ifndef COMP_TOAST_HPP_
#define COMP_TOAST_HPP_
#include<QWidget>
#include<QDialog>
#include<QLabel>
#include<QHBoxLayout>
#include<QEvent>
extern QString get_wrap_word(const QFontMetrics&font,const QString&text,int limitSize);  
namespace qt_elevencent{
  class CompToast:public QWidget{
    Q_OBJECT
  private:
    int m_timerId=0;
  public:
    QLabel*m_label;
    QHBoxLayout*m_hLayout;
    CompToast(QWidget*parent=nullptr):QWidget(parent){
      using namespace std;
      setStyleSheet("background-color:black;");
      setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
      setContentsMargins(0,0,0,0);
      m_hLayout=new QHBoxLayout(this);
      m_hLayout->setContentsMargins(0,0,0,0);
      m_hLayout->setSpacing(0);
      setLayout(m_hLayout);
      m_label=new QLabel(this);
      m_label->setContentsMargins(0,0,0,0);
      m_label->setIndent(0);      
      m_label->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
      m_label->setAlignment(Qt::AlignVCenter);
      m_label->setStyleSheet("background-color:#2B2B2B;font:19px;color:white;border-radius:4px;");
      m_label->setIndent(0);      
      m_hLayout->addWidget(m_label,1);
      hide();
    }
    void adjust2center(){
      if(!parentWidget())return;
      QSize psz=parentWidget()->size();
      int h=psz.height()/6,x=10,
	y=psz.height()-h*2,w=psz.width()-x*2;
      setGeometry(x,y,w,h);
    }
    void timerEvent(QTimerEvent*ev){
      killTimer(m_timerId);
      m_timerId=0;
      hide();
    }
    void show(QString&text,int time){
      show(std::forward<QString>(text),time);
    }
    void show(QString&&text,int time){
      adjust2center();
      QWidget::show();
      raise();
      m_label->setText(get_wrap_word(QFontMetrics(m_label->font()),std::forward<QString>(text),m_label->size().width()));
      if(m_timerId)
	killTimer(m_timerId);
      m_timerId=startTimer(time);
    }
  };
}

#endif
