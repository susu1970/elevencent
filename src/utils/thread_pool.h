#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include<functional>
#include<pthread.h>

namespace elevencent{
  class ThreadPool;
  class ThreadPoolNode{
    friend ThreadPool;
  private:
    short m_nice;
    std::function<void*(void*)>m_task;
    std::function<void*(void*)>m_callback;
    int m_id;
    ThreadPoolNode*m_pLeft;
    ThreadPoolNode*m_pRight;
    ThreadPoolNode*m_pParent;
  public:
    ThreadPoolNode(short nice=10);
  };
  class ThreadPool{
  private:
    bool m_isNiceEnabled;
    pthread_mutex_t m_mutex;
    unsigned short m_num;
  private:
    void addThread(unsigned short num=1);
    void rmThread(unsigned short num=1);
  public:
    ThreadPool(bool enableNice=true);
    int pushTask(std::function<void*(void*)>task,void*arg,std::function<void*(void*)>callback, short nice=10);    
    bool getIsNiceEnabled();
    void setIsNiceEnabled(bool);
     void resume();
    void start();
  };
}

#endif
















