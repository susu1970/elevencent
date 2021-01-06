#ifndef ELEVENCENT_LIST_HPP_
#define ELEVENCENT_LIST_HPP_

#include<utility>

#include"global.h"

namespace elevencent{

  template<typename T>
  class Node{
  public:
    T m_data;
    Node*m_next;
  public:
    Node(T&&data):m_data(std::forward(data)),m_next(nullptr){DEBUG_MSG("");}
    Node(T&data):m_data(std::forward(data)),m_next(nullptr){DEBUG_MSG("");}
    Node(T data):m_data(data),m_next(nullptr){DEBUG_MSG("");}
  };
  
  template<typename T>
  class List{
  public:
    Node*m_tail;
    size_t m_size;
  public:
    List():m_tail(nullptr),m_size(0){}
    inline void push_back(T&data);
    inline void push_back(T&&data);
    inline void push_back(T data);
    inline T pop_front();
    inline T&pop_front();
    inline T&front();
    inline size_t size(){return m_size;}
    inline bool empty(){return m_size==0;}
    ~List();
  };

#define LIST_PUSH_BACK ({						\
      Node<T>*p=new Node<T>(std::forward(data));				\
      ++m_size;								\
      if(!m_tail){							\
	m_tail=p;							\
	m_tail->next=m_tail;						\
	return;								\
      }									\
      p->next=m_tail->next;						\
      m_tail->next=p;							\
      m_tail=p;								\
    })

  template<typename T>
  void List<T>::push_back(T&data){
    LIST_PUSH_BACK;
  }
  
  template<typename T>
  void List<T>::push_back(T&&data){
    LIST_PUSH_BACK;
  }
  
  template<typename T>
  void List<T>::push_back(T data){
    LIST_PUSH_BACK;
  }
  
  template<typename T>
  T&List::front(){
    if(m_size==0)return 0;
    return m_tail->next->m_data;
  }

  template<typename T>
  List::~List(){
    DEBUG_MSG("");
    auto p=m_tail;
    while(p){
      auto q=p->next;
      delete p;
      if(q==m_tail)break;
      p=q;
    }
  }

}



#endif
















