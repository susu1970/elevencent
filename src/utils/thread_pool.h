#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include<list>
#include<functional>
#include<pthread.h>

#include"global.h"

namespace elevencent{
  enum TaskNice:short{
    TaskNiceEmerge,
    TaskNiceLow,
    TaskNiceDft,
    TaskNiceHigh
  };
  
  
  class ThreadPool{
  private:
    
    class TaskNode{
      friend ThreadPool;
    private:
      std::function<void*(void*)>m_task;
      void*m_arg;
      std::function<void(void*)>m_callback;
      short m_nice;
      short m_idx;
      TaskNode*m_left,*m_right,*m_parent;    
    private:
      void doTask();
      TaskNode(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback,short nice,short idx=-1);
    };

  private:
    bool m_niceon;
    short m_idle,m_busy,m_tasks;
    TaskNode*m_head,*m_tail;
    TaskNode*m_tpr;//tail-parent-right
  private:

  public:
    ThreadPool(bool niceon=false);
    void pushTask(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback,short nice=TaskNice::TaskNiceDft);
    inline void setNiceon(bool niceon);
    inline bool niceon();
    void run();
    void consumeTask(short num=1);
    void traverseLayer();
  };
}

#endif


















