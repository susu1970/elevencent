#ifndef ELEVENCENT_LIST_HPP_
#define ELEVENCENT_LIST_HPP_

#include<utility>

#include"global.h"

namespace elevencent{
  template<typename T>
  class List{
  private:
    class Node{
      friend List;
    private:
      T m_data;
      Node*m_next;
    private:
      Node(T&&data):m_data(std::forward(data)),m_next(nullptr){}
      Node(T&data):m_data(std::forward(data)),m_next(nullptr){}
    };
    
  private:
    Node*m_tail;
  public:
    List():m_tail(nullptr){}
    void push_back(T&data);
    void push_back(T&&data);
    void move(List&target,T&&data);
    T&pop();
    void clear();
  };

  
  template<typename T>
  void List<T>::push_back(T&data){
    Node*p=new Node(data);
    if(!m_tail){
      m_tail=p;
      m_tail->next=m_tail;
      return;
    }
    p->next=m_tail->next;
    m_tail->next=p;
    m_tail=p;
  }
  
  template<typename T>
  void List<T>::push_back(T&&data){
    Node*p=new Node(std::forward(data));
    if(!m_tail){
      m_tail=p;
      m_tail->next=m_tail;
      return;
    }
    p->next=m_tail->next;
    m_tail->next=p;
    m_tail=p;    
  }
  
  
}



#endif
















