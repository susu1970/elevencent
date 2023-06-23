#include"resource_view.h"
#include"global.h"
#include<string>
#include<iostream>
#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QSet>
#include <QPointF>
#include <QRandomGenerator>

#define POINTS_NUMBER 4000
#define FREQUENCY 40
#define DANCESIZE 50
#define IMAGE_ENLARGE 15

using namespace qt_elevencent;
using namespace std;
QPointF heart_function(qreal t,qreal shrink_ratio=IMAGE_ENLARGE){
  qreal x=16*(sin(t)*sin(t)*sin(t));
  qreal y=-(13*cos(t)-5*cos(2*t)-2*cos(3*t)-cos(4*t));
  x*=shrink_ratio;
  y*=shrink_ratio;
  return QPointF(x,y);
}
QPointF scatter_inside(QPointF&point,qreal beta=0.05){
  qreal ratiox=-log(QRandomGenerator::global()->bounded(1.0))*beta;
  qreal ratioy=-log(QRandomGenerator::global()->bounded(1.0))*beta;
  qreal dx=point.x()*ratiox;
  qreal dy=point.y()*ratioy;
  return QPointF(point.x()-dx,point.y()-dy);
}
QPointF caculate_position(QPointF&point,qreal ratio=0.9){
  qreal force=1/qPow((point.x()*point.x()+point.y()*point.y()),0.6);
  qreal dx=point.x()*force*ratio+QRandomGenerator::global()->bounded(-1,1);
  qreal dy=point.y()*force*ratio+QRandomGenerator::global()->bounded(-1,1);
  return QPointF(point.x()-dx,point.y()-dy);
}
QPointF shrink_ratio(QPointF&point,qreal ratio=0.9){
  qreal force=1/qPow((point.x()*point.x()+point.y()*point.y()),1);
  qreal dx=point.x()*force*ratio;
  qreal dy=point.y()*force*ratio;
  return QPointF(point.x()-dx,point.y()-dy);
}
qreal curve(qreal p){
  return 2*(2*sin(4*p))/(2*M_PI);
}
ResourceView::ResourceView(QWidget*parent):QWidget{parent}{
  setStyleSheet("background-color:black;");
  setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);      
  setContentsMargins(0,0,0,0);
  m_vLayout=new QVBoxLayout(this);
  m_vLayout->setSpacing(0);
  m_vLayout->setContentsMargins(0,0,0,0);
  setLayout(m_vLayout);
#if 0
  m_searchBar=new CompSearchBar(this);
  m_searchBar->setContentsMargins(2,2,2,2);
  m_searchBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  
  m_vLayout->addWidget(m_searchBar);
  m_vLayout->setStretchFactor(m_searchBar,1);        
  m_searchBar->show();  

  m_contentV=new CompContentListView(this);
  m_vLayout->addWidget(m_contentV);
  m_vLayout->setStretchFactor(m_contentV,12);
  m_contentV->show();
#endif
  initPoints();
  timer=new QTimer(this);
  connect(timer,&QTimer::timeout,this,qOverload<>(&ResourceView::update));
  timer->start(1000/FREQUENCY);  
}
void ResourceView::paintEvent(QPaintEvent *event) {
  initPoints();    
  qreal ratio=DANCESIZE*curve(frame/(double)FREQUENCY*M_PI);
  QPolygonF points;
  #define PFS_NUM 40000
  QPolygonF pfs[PFS_NUM];
  int pfsidx=0;
  qint32 halo_radius=int(4+6*(1+curve(frame/(double)FREQUENCY*M_PI)));
  qint32 halo_number=int(3000+4000*abs(curve(frame/(double)FREQUENCY*M_PI)*curve(frame/(double)FREQUENCY*M_PI)));
  for(int i=0;i<halo_number;++i) {
    qreal t=QRandomGenerator::global()->bounded(2*M_PI);
    QPointF p=heart_function(t,IMAGE_ENLARGE);
    QPointF pp=shrink_ratio(p,halo_radius);
    pp.setX(pp.x()+QRandomGenerator::global()->bounded(-14,14));
    pp.setY(pp.y()+QRandomGenerator::global()->bounded(-14,14));
    points.append(pp);
    pfs[pfsidx].append(pp);
    if(++pfsidx>=PFS_NUM)
      pfsidx=0;
  }
  for(int i=0;i<POINTS_NUMBER;++i){
    QPointF p=m_points.at(i);
    p=caculate_position(p,ratio);
    points.append(p);
    pfs[pfsidx].append(p);
    if(++pfsidx>=PFS_NUM)
      pfsidx=0;    
  }
  for(int i=0;i<m_edge_diffusion_points.size();++i) {
    QPointF p=m_edge_diffusion_points.at(i);
    p=caculate_position(p,ratio);
    points.append(p);
    pfs[pfsidx].append(p);
    if(++pfsidx>=PFS_NUM)
      pfsidx=0;    
  }
  for(int i=0;i<m_center_diffusion_points.size();++i) {
    QPointF p=m_center_diffusion_points.at(i);
    p=caculate_position(p,ratio);
    points.append(p);
    pfs[pfsidx].append(p);
    if(++pfsidx>=PFS_NUM)
      pfsidx=0;    
  }
  for(int i=0;i<PFS_NUM;++i){
    QPainter pfsp(this);
    int h=rand()%254+1;
    int s=rand()%254+1;
    int v=rand()%254+1;
    pfsp.setRenderHint(QPainter::Antialiasing,true);
    pfsp.setPen(QPen(QColor::fromHsl(h,s,v)));
    pfsp.save();
    pfsp.translate(width()/2,height()/2);
    pfsp.drawPoints(pfs[i]);
    pfsp.restore();
  }
  {
    QPainter pfsp(this);
    int h=rand()%254+1;
    int s=rand()%254+1;
    int v=rand()%254+1;
    pfsp.setRenderHint(QPainter::Antialiasing,true);
    pfsp.setPen(QPen(QColor::fromHsl(h,s,v)));
    pfsp.save();
    pfsp.translate(width()/2,height()/2);
    pfsp.drawPoints(m_points);
    pfsp.restore();    
  }
  frame++;

}
void ResourceView::resizeEvent(QResizeEvent*event) {
  update();
  initPoints();    
}
void ResourceView::initPoints() {
  m_points.clear();
  m_edge_diffusion_points.clear();
  m_center_diffusion_points.clear();
  for(int i=0;i<POINTS_NUMBER;++i) {
    qreal t=QRandomGenerator::global()->bounded(2*M_PI);
    QPointF p=heart_function(t);
    m_points.append(p);
  }
  for(int i=0;i<POINTS_NUMBER;++i) {
    QPointF p=m_points.at(i);
    for(int j=0;j<3;++j){
      QPointF s=scatter_inside(p, 0.05);
      m_edge_diffusion_points.append(s);
    }
  }
  for(int i=0;i<POINTS_NUMBER*2;++i) {
    qint32 index=QRandomGenerator::global()->bounded(POINTS_NUMBER);
    QPointF p=m_points.at(index);
    QPointF s=scatter_inside(p,0.17);
    m_center_diffusion_points.append(s);
  }
}
