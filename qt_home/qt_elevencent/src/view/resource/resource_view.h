#ifndef _RESOURCE_VIEW_H_
#define _RESOURCE_VIEW_H_

#include"comp_search_bar.hpp"
#include"comp_content_list_view.hpp"
#include <QWidget>
#include <QRandomGenerator>

namespace qt_elevencent{
  class ResourceView:public QWidget{
    Q_OBJECT
  public:
    explicit ResourceView(QWidget*parent=nullptr);
    void paintEvent(QPaintEvent*event);
    void resizeEvent(QResizeEvent*event);
  private:
    qint64 frame=0;
    QTimer*timer;
    QList<QPointF>m_points; 
    QList<QPointF>m_edge_diffusion_points;
    QList<QPointF>m_center_diffusion_points;
    void initPoints();
  private:
    QVBoxLayout*m_vLayout;      
    CompContentListView*m_contentV;
    CompSearchBar*m_searchBar;
  };
}

#endif
