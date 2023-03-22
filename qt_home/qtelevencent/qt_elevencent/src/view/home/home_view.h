#ifndef HOME_VIEW_H_
#define HOME_VIEW_H_

#include<QHBoxLayout>
#include<QVBoxLayout>
#include<unordered_map>
#include<QWidget>
#include<QPushButton>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QButtonGroup>

#include"message_view.h"
#include"resource_view.h"
#include"setting_view.h"
namespace qt_elevencent{
  class HomeBottomView;
  class HomeContentView;  
  enum class HOME_BOTTOM_IDX{
    MESSAGE=0,
    RESOURCE,
    SETTING,
    END
  };    
  class HomeView:public QWidget{
    Q_OBJECT
  public:
    explicit HomeView(QWidget *parent = nullptr);
  signals:
    
  private:
  private:
    HomeBottomView*m_hbv;
    HomeContentView*m_hcv;  
    QVBoxLayout*m_vLayout;
  public slots:
    void onBottomClicked(int idx);            
  };
  class HomeBottomView:public QWidget{
    friend HomeView;
    Q_OBJECT
  public:
    explicit HomeBottomView(QWidget*parent=nullptr);
  signals:
    
  private:
    QButtonGroup*m_btnGroup;
    QHBoxLayout*m_hLayout;
  public:
  public slots:
    void onBtnGroupClicked(int idx);        
  };
  class HomeContentView:public QWidget{
    friend HomeView;      
    Q_OBJECT
  public:
    explicit HomeContentView(QWidget *parent = nullptr);
    void showByIdx(HOME_BOTTOM_IDX idx);
    void hideByIdx(HOME_BOTTOM_IDX idx=HOME_BOTTOM_IDX::END);
  signals:
  private:
    std::unordered_map<HOME_BOTTOM_IDX,QWidget*>m_contentUM;
    MsgView*m_msgV;
    ResourceView*m_resourceV;
    SettingView*m_settingV;
    QHBoxLayout*m_hLayout;      
  };  
}
#endif
