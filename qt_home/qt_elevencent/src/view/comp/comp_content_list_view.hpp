#ifndef COMP_CONTENT_LIST_VIEW_HPP_
#define COMP_CONTENT_LIST_VIEW_HPP_

#include<QWidget>
#include<QLabel>
#include<QtGui>
#include<QVBoxLayout>
#include<QLineEdit>
#include<QPushButton>
#include<QDebug>
#include<QListView>
#include<QStandardItemModel>
#include<vector>
#include<QString>
#include<QStyledItemDelegate>

namespace qt_elevencent{
  class CompContentListItemData{
  public:
    QString m_iconPath="";
    QString m_name="";
    CompContentListItemData(QString name="",QString iconPath=""):m_name{name},m_iconPath{iconPath}{}
    CompContentListItemData(const CompContentListItemData&e):m_iconPath(e.m_iconPath),m_name(e.m_name){}
  };
  class CompContentListDelegate:public QStyledItemDelegate{
    Q_OBJECT
  public:
    CompContentListDelegate(QObject*parent=0):QStyledItemDelegate(parent){}
    void paint(QPainter*painter,const QStyleOptionViewItem&option,const QModelIndex&index)const{

      if(!index.data(Qt::DisplayRole).canConvert<CompContentListItemData>())
	return QStyledItemDelegate::paint(painter,option,index);      
      const QRect&rect=option.rect;      
      auto itemData=(index.data(Qt::DisplayRole).value<CompContentListItemData>());
      {
	painter->save();
	painter->setPen(QColor("#212f3c"));
	painter->drawRect(rect.x()+20,rect.y()+rect.height()-1,rect.width()-4*20,1);
	painter->restore();
      }
      {
	painter->save();
        QFont font;
        font.setFamily("Microsoft YaHei");
        font.setPixelSize(32);
        painter->setFont(font);
        QRect txtRec(rect.x()+22,rect.y(),rect.width(),rect.height());
        painter->drawText(txtRec,Qt::AlignLeft,itemData.m_name);
	painter->restore();
      }
    }
    QSize sizeHint(const QStyleOptionViewItem&option,const QModelIndex&index){
      return QSize(500,500);
      //      return QStyledItemDelegate::sizeHint(option,index);
    }        
  };
  class CompContentListModel:public QAbstractListModel{  
    Q_OBJECT  
  public:  
    CompContentListModel(QObject*parent=NULL):QAbstractListModel{parent}{}
    ~CompContentListModel(){
      for(auto e:m_datas)
	delete e;
    }
    void addItem(const CompContentListItemData&data){
      beginInsertRows(QModelIndex(),m_datas.size(),m_datas.size()+1);
      m_datas.push_back(new CompContentListItemData(data));  
      endInsertRows();  
    }
    QVariant data(const QModelIndex&index,int role=Qt::DisplayRole)const{
      if(index.row()>=m_datas.size())
	return QVariant();
      switch(role){  
      case Qt::DisplayRole:
	return QVariant::fromValue(*(m_datas[index.row()]));
	break;
      case Qt::DecorationRole:
	return QVariant::fromValue(*(m_datas[index.row()]));	
	break;
      default:
	break;
      }
      return QVariant();
    }
    bool setData(const QModelIndex&index,const QVariant&value,int role){
      CompContentListItemData*itemData=index.data().value<qt_elevencent::CompContentListItemData*>();
      qDebug()<<"row: "<<index.row()<<", icon: "<<itemData->m_iconPath<<", name: "<<itemData->m_name;
      return true;
    }
    int rowCount(const QModelIndex&parent=QModelIndex())const{
      return m_datas.size();
    }
    void deleteItem(int index){
      m_datas.erase(m_datas.begin()+index);  
    }
    CompContentListItemData*getItem(int index){
      if(index<0||index>=m_datas.size())
	return 0;
      return m_datas[index];
    }
  protected:  
  private:  
    std::vector<CompContentListItemData*> m_datas;  
  };    
  class CompContentListView:public QListView{
    Q_OBJECT
  public:
    explicit CompContentListView(QWidget*parent=nullptr):QListView{parent}{
      using namespace std;
      setStyleSheet("background-color:#282A36;border-radius:6;font:32px;color:#a97bbe;border:0px solid black;font-family:Microsoft YaHei;");
      setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
      setEditTriggers(QAbstractItemView::NoEditTriggers);
      setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      setContentsMargins(0,0,0,0);
      setSpacing(1);
      setModel(&m_model);
      setItemDelegate(&m_delegate);
    }
    void addItem(const CompContentListItemData&data){
      m_model.addItem(data);
    }    
  public slots:
  public:
    CompContentListModel m_model;
    CompContentListDelegate m_delegate;    
  };

}
Q_DECLARE_METATYPE(qt_elevencent::CompContentListItemData);
#endif
