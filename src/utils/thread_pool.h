#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include<list>
#include<functional>
#include<pthread.h>

#include"global.h"

namespace elevencent{
  enum class TaskNice:short{
    low=10,
    dft=50,
    high=70,
  };
  
  class ThreadPoolNode{
  public:
    std::function<void*(void*)>*m_task;
    void*m_arg;
    std::function<void(void*)>*m_callback;
    TaskNice m_nice;
    ThreadPoolNode*m_left,*m_right,*m_parent;
  public:
    void doTask();
    explicit ThreadPoolNode();
    ThreadPoolNode(std::function<void*(void*)>*task,void*arg,std::function<void(void*)>*callback=nullptr,TaskNice nice=TaskNice::dft);
  };
  /*
  class ThreadPool{
    friend std::ostream&operator<<(ostream&,ThreadPool&);
  private:
    void delThread(short num=1);
    void addThread(short num=2);
    void*thr(void*arg);
    void thrcleanup(void*arg);
    void consumeTask(short num=1);
    ThreadPoolNode*popTask();
  private:
    pthread_mutex_t m_mutex,m_cond_mutex;
    pthread_cond_t m_cond;
    bool m_niceon;
    short m_idle,m_busy,m_maxbusy,m_minidle,m_maxidle;
    std::list<pthread_t>m_idlethrs,m_busythrs;
  public:
    ThreadPool(short minidle=2,short maxidle=8,short maxbusy=17,bool niceon=false);
  };
  */
}

#endif


















