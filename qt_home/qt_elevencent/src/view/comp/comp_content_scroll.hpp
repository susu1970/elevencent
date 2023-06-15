#ifndef COMP_CONTENT_SCROLL_HPP_
#define COMP_CONTENT_SCROLL_HPP_

#include<QScrollArea>
#include<QLabel>
#include<QVBoxLayout>
#include<QScrollBar>
#include<string>
#include<iostream>
#include<QDebug>
extern QString str2html(const QString&&data);      
extern QString str2html(const QString&data);
namespace qt_elevencent{
  class CompContentScroll:public QScrollArea{
    Q_OBJECT
  private:
    QLabel*m_label;
    int m_maxLines;
  public:
    explicit CompContentScroll(QWidget*parent=0):QScrollArea(parent){
      using namespace std;
      setStyleSheet("background-color:#282A36;font: 30px;color:#282A36;border-radius:4;");
      setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
      setWidgetResizable(true);
      setContentsMargins(0,0,0,0);
      setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
      setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);      
      horizontalScrollBar()->setStyleSheet
	("QScrollBar:horizontal{"
	 "background:#282a36;"  //背景色
	 "padding-top:0px;"    //上预留位置（放置向上箭头）
	 "padding-bottom:0px;" //下预留位置（放置向下箭头）  
	 "padding-left:0px;"    //左预留位置（美观）
	 "height:5px;"
	 "padding-right:0px;}"   //右预留位置（美观）  
	 "QScrollBar::handle:horizontal{"//滑块样式  
	 "background:gray;"  //滑块颜色
	 "border-radius:5px;" //边角圆润  
	 "min-width:20px;}"    //滑块最小高度  
	 "QScrollBar::sub-line:horizontal{"//向下箭头样式  
	 "background:none;}"  
	 "QScrollBar::add-line:horizontal{"//向上箭头样式  
	 "background:none;}");       
      verticalScrollBar()->setStyleSheet
	("QScrollBar:vertical{"
	 "background:#282a36;"  //背景色
	 "padding-top:0px;"    //上预留位置（放置向上箭头）
	 "padding-bottom:0px;" //下预留位置（放置向下箭头）  
	 "padding-left:0px;"    //左预留位置（美观）
	 "width:5px;"    
	 "padding-right:0px;}"   //右预留位置（美观）  
	 "QScrollBar::handle:vertical{"//滑块样式  
	 "background:gray;"  //滑块颜色
	 "border-radius:2px;" //边角圆润  
	 "min-height:20px;}"    //滑块最小高度  
	 "QScrollBar::add-line:vertical{"//向下箭头样式  
	 "background:none;}"  
	 "QScrollBar::sub-line:vertical{"//向上箭头样式  
	 "background:none;}");
      m_label=new QLabel(this);
      setWidget(m_label);
      m_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      m_label->setTextFormat(Qt::RichText);
      m_label->setStyleSheet("background-color:#282a36;color:gray;font-family:Microsoft YaHei;font: 21px;");
      m_label->setAlignment(Qt::AlignLeft|Qt::AlignTop);
      m_label->setMargin(0);
      m_label->setContentsMargins(0,0,0,0);
      m_label->setTextInteractionFlags(Qt::TextSelectableByMouse);
      m_label->setIndent(0);
      m_label->setWordWrap(true);
      m_label->adjustSize();
      m_label->show();
    }
    void setText(const QString&&plainText,const QString&&preFormat="",const QString&&tailFormat=""){
      m_label->setText(preFormat+str2html(plainText)+tailFormat);
      verticalScrollBar()->setValue(verticalScrollBar()->maximum());
    }
    void appendText(const QString&&plainText,const QString&&preFormat="",QString&&tailFormat=""){
      m_label->setText(m_label->text()+preFormat+str2html(plainText)+tailFormat);
      verticalScrollBar()->setValue(verticalScrollBar()->maximum());
    }
  };
}

#endif
