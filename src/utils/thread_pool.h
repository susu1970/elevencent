#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include<functional>
#include<pthread.h>
#include<vector>

namespace elevencent{
  class ThreadPoolNode{
  public:
    ThreadPoolNode();
  public:
    short m_nice;
    ThreadPoolNode*m_parent,*m_left,*m_right;
    std::function<void*(void*)>m_task;
    std::function<void(void*)>m_callback;
  };

  class ThreadPool{
  private:
    
  public:
    ThreadPool();
  public:
    
  };
}

#endif

















