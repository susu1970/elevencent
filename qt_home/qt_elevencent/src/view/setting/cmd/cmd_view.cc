#include"app.h"
#include"cmd_view.h"
#include"cmd_model.h"
#include"global.h"
#include<string>
#include<string>
#include<iostream>
#include<QCommandLineParser>
#include<QFontMetrics>

using namespace qt_elevencent;
using namespace std;
extern int str2arg(const QString&s,char*argv[]);
void CmdView::paintEvent(QPaintEvent*ev){
  QPainter p(this);
  p.setPen(QColor("black"));
  p.drawRect(10,m_hintScroll->y()+m_hintScroll->height(),m_hintScroll->width()-2*10,0);
}
CmdView::CmdView(QWidget*parent):QWidget{parent}{
  setStyleSheet("background-color:#282a36;");
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setContentsMargins(0,0,0,0);  
  m_vLayout=new QVBoxLayout(this);
  m_vLayout->setContentsMargins(0,0,0,0);
  m_vLayout->setSpacing(2);      
  setLayout(m_vLayout);
  m_searchBar=new CompSearchBar(this);
  m_vLayout->addWidget(m_searchBar);
  m_vLayout->setStretchFactor(m_searchBar,1);
  m_searchBar->m_searchBtn->hide();
  m_searchBar->show();  
  m_searchBar->m_hLayout->setSpacing(0);  

  m_hintScroll=new CompContentScroll(this);

  m_vLayout->addWidget(m_hintScroll);
  m_vLayout->setStretchFactor(m_hintScroll,3);
  m_hintScroll->show();

  m_contentScroll=new CompContentScroll(this);
  m_vLayout->addWidget(m_contentScroll);
  m_vLayout->setStretchFactor(m_contentScroll,9);
  m_contentScroll->show();
  connect(m_searchBar->m_searchBtn,SIGNAL(clicked()),this,SLOT(onSearchBarDoSearch()));
  connect(m_searchBar->m_searchEdit,SIGNAL(returnPressed()),this,SLOT(onSearchBarDoSearch()));
}
void CmdView::onSearchBarDoSearch(){
  const QString text=m_searchBar->m_searchEdit->text();  
  if(text.size()>=MAX_CMD_ARGV){
    output(CMD_VIEW_CONTENT_IDX::HINT,QString("text.size(): "+QString::number(text.size())+" >= MAX_CMD_ARGV ("+QString::number(MAX_CMD_ARGV)+"\n"));
    return;
  }
  CmdModel cmd;
  for(int i=0;i<MAX_CMD_ARGC;++i)
    cmd.pargv[i]=cmd.argv[i];
  cmd.argc=str2arg(text,cmd.pargv);
  output(CMD_VIEW_CONTENT_IDX::HINT,QString("argc:"+QString::number(cmd.argc)+"\n"));
  if(cmd.argc<=0){
    output(CMD_VIEW_CONTENT_IDX::HINT,QString("cmd line parse error:\n"));
    int cur_pos=-cmd.argc;
    int prev_len=20;
    int prev_pos=MAX(cur_pos-prev_len,0);
    prev_len=cur_pos-prev_pos;
    int next_len=20;
    output(CMD_VIEW_CONTENT_IDX::HINT,text.mid(prev_pos,prev_len));
    output(CMD_VIEW_CONTENT_IDX::HINT,text.mid(cur_pos,1),true,"<font size='+1' color=#4285F4><b>","</b></font>");    
    if(cur_pos+1<text.size())
      output(CMD_VIEW_CONTENT_IDX::HINT,text.mid(cur_pos+1,next_len));
    output(CMD_VIEW_CONTENT_IDX::HINT,"\n");
    return;
  }
  for(int i=0;i<cmd.argc;++i)
    output(CMD_VIEW_CONTENT_IDX::HINT,QString("argv["+QString::number(i)+"]:"+cmd.pargv[i]+"\n"));
  App::getInstance()->sendEvent(EVENT_TYPE_CMD_MAIN,&cmd);
}
void CmdView::output(CMD_VIEW_CONTENT_IDX target,QString&&text,bool append,QString&&preFormat,QString&&tailFormat){
  CompContentScroll*content=0;
  if(target==CMD_VIEW_CONTENT_IDX::HINT)
    content=m_hintScroll;
  else if(target==CMD_VIEW_CONTENT_IDX::CONTENT)
    content=m_contentScroll;
  else
    return;
  if(append)
    content->appendText(forward<QString>(text),forward<QString>(preFormat),forward<QString>(tailFormat));
  else
    content->setText(forward<QString>(text),forward<QString>(preFormat),forward<QString>(tailFormat));    
}
void CmdView::output(CmdViewOutputArg&&arg){
  output(arg.m_target,forward<QString>(arg.m_text),arg.m_append,forward<QString>(arg.m_preFormat),forward<QString>(arg.m_tailFormat));
}
