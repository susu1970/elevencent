#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include<list>
#include<functional>
#include<pthread.h>
#include<map>

#include"global.h"

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
      void doTask(){m_callback(m_task(m_arg));}
      TaskNode(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback,short nice,short idx=-1):m_nice(nice),m_task(std::forward<std::function<void*(void*)>>(task)),m_arg(arg),m_callback(std::forward<std::function<void(void*)>>(callback)),m_left(nullptr),m_right(nullptr),m_parent(nullptr),m_idx(idx){}
    };
    TaskNode*m_head,*m_tail;
    TaskNode*m_tpr;//tail-parent-right
    static void*thrFunc(void*);
    static void thrCleanup(void*);
    static void thrCleanup1(void*);
    static void thrCleanup2(void*);        
    static void thrCleanup3(void*);
    static void*clearAllThrsFunc(void*);
    void createTaskHandler(short num=1);
    TaskNode*popTask();
    void updateThrData(short*thrDatas);
    void pushTaskNode(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback,short nice=TaskNice::TaskNiceDft);
    bool m_niceon;
    short m_thrIdle,m_thrBusy,m_tasks,m_curThrNum,m_maxTasks;
    pthread_mutex_t m_taskMutex,m_maxTaskMutex,m_curThrNumMutex,m_thrIdleMutex,m_thrBusyMutex,m_thrtListMutex,m_cancelMutex;
    pthread_cond_t m_taskCond,m_maxTaskCond,m_curThrNumZeroCond;
    pthread_attr_t m_thrAttr;
    std::unordered_map<pthread_t,bool>m_cancelMap;    
    std::function<void(ThreadPool*,short*thrDatas)>m_updateThrData;
    std::function<void(void*arg)>m_clearAllThrsCb;
    std::list<pthread_t>m_thrtList;
    void*m_clearAllThrsCbArg;
  public:
    void wasteAllTasks();    
    void consumeTask(short num=1);
    void consumeAllTasks();
    void clearAllThrs(std::function<void(void*arg)>&&cb,void*arg=nullptr,bool wasteTasks=false);
    void clearAllThrs(bool wasteTasks=false);    
    ThreadPool(bool niceon=true);
    ThreadPool(std::function<void(ThreadPool*,short*)>&&updateThrData,short maxTasks=32000,bool niceon=true);
    ~ThreadPool();
    void pushTask(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback,short nice=TaskNice::TaskNiceDft);
    void traverseLayer();
    bool niceon(){return m_niceon;}
    void setThrDataFunc(std::function<void(ThreadPool*,short*thrDatas)>&&updateThrData);
    void test1();
  };
}

#endif


















