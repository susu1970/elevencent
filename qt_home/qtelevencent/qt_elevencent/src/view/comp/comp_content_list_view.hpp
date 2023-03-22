#ifndef COMP_CONTENT_LIST_VIEW_HPP_
#define COMP_CONTENT_LIST_VIEW_HPP_

#include<QWidget>
#include<QVBoxLayout>
#include<QLineEdit>
#include<QPushButton>
#include<QDebug>
#include<QListView>
#include<QStandardItemModel>
#include<vector>
#include<QString>

namespace qt_elevencent{
  class CompContentListItemData{
  public:
    QString m_iconPath="";
    QString m_name="";
    CompContentListItemData(QString name="",QString iconPath=""):m_name{name},m_iconPath{iconPath}{}
  };
  class CompContentListModel:public QAbstractListModel{  
    Q_OBJECT  
  public:  
    CompContentListModel(QObject*parent=NULL):QAbstractListModel{parent}{}
    ~CompContentListModel(){
      for(auto e:m_datas)
	delete e;
    }
    void addItem(CompContentListItemData*data){
      if(!data)
	return;
      beginInsertRows(QModelIndex(),m_datas.size(),m_datas.size()+1);  
      m_datas.push_back(data);  
      endInsertRows();  
    }
    QVariant data(const QModelIndex&index,int role=Qt::DisplayRole)const{
      if(index.row()>=m_datas.size())
	return QVariant();
      switch(role){  
	case Qt::DisplayRole:  
	  return m_datas[index.row()]->m_name;  
	case Qt::DecorationRole:  
	  return QIcon(m_datas[index.row()]->m_iconPath);
      default:
	break;
      }  
      return QVariant();        
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
      setStyleSheet("background-color:#282A36;font-weight:bold;font: bold 40px;color:#4285F4;border-radius:6;");
      setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
      setEditTriggers(QAbstractItemView::NoEditTriggers);
      setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);      
      m_model=new CompContentListModel();
      setModel(m_model);
    }
    void addItem(CompContentListItemData*data){
      m_model->addItem(data);
    }    
  public slots:
  public:
    CompContentListModel*m_model;
  };

}

#endif
