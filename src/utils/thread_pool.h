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
    void*m_arg;
    std::function<void*(void*)>m_callback;
    int m_id;
    ThreadPoolNode*m_left;
    ThreadPoolNode*m_right;
    ThreadPoolNode*m_parent;
  public:
    ThreadPoolNode(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback,short nice=10);
    ThreadPoolNode(std::function<void*(void*)>&&task,void*arg,short nice=10);
  };
  
  class ThreadPool{
  private:
    bool m_niceEnabled;
    short m_minIdle;
    short m_maxBusy;
    pthread_mutex_t m_mutex;
    short m_num;
    short m_curIdle;
    bool m_running;
    ThreadPoolNode*m_root;
  private:
    void add_thread(unsigned short num=1);
    void rm_thread(unsigned short num=1);
  public:
    ThreadPool(bool enableNice=false,short minIdle=3,short maxBusy=17);
    int push(ThreadPoolNode*node);
    int push(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback,short nice=10);
    bool getNiceEnabled(){return m_niceEnabled;}
    void run();
    void pause();
    ~ThreadPool();
  };
}

#endif
















