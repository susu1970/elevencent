#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include<list>
#include<functional>
#include<pthread.h>

#include"global.h"

#define THR_POOL_IDX_CACHED_NUM 0
#define THR_POOL_IDX_MAX_NUM 1

namespace elevencent{
  enum TaskNice:short{
    TaskNiceEmerge,
    TaskNiceLow,
    TaskNiceDft,
    TaskNiceHigh
  };

  enum ThrDataIdx:char{
    ThrDataIdxCached,
    ThrDataIdxMax,

    ThrDataIdxEnd
  };
  
  class ThreadPool final{
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
      void doTask(){m_callback(m_task(m_arg));}
      TaskNode(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback,short nice,short idx=-1):m_nice(nice),m_task(forward<function<void*(void*)>>(task)),m_arg(arg),m_callback(forward<function<void(void*)>>(callback)),m_left(nullptr),m_right(nullptr),m_parent(nullptr),m_idx(idx){}
    };
    TaskNode*m_head,*m_tail;
    TaskNode*m_tpr;//tail-parent-right
    bool m_maxTaskMet;
  private:
    void createTaskHandler(short num=1);
    TaskNode*popTask();
    void pushTaskNode(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback,short nice=TaskNice::TaskNiceDft);
    void consumeTask(short num=1);    
  private:
    bool m_niceon;
    short m_thrIdle,m_thrBusy,m_tasks,m_curThrNum,m_maxTasks;
    pthread_mutex_t m_taskMutex,m_maxTaskMutex,m_curThrNumMutex,m_thrIdleMutex,m_thrBusyMutex;
    pthread_cond_t m_taskCond,m_maxTaskCond;
  public:
    ThreadPool(std::function<void(ThreadPool*,short*)>&&updateThrData,short maxTasks=32000,bool niceon=true);
    void pushTask(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback,short nice=TaskNice::TaskNiceDft);
    void traverseLayer();
  public://getter
    bool niceon(){return m_niceon;}
  public:
    std::function<void(ThreadPool*,short*thrDatas)>m_updateThrData;
  };
}

#endif


















