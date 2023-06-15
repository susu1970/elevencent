#ifndef THREAD_POOL_RUDE_H_
#define THREAD_POOL_RUDE_H_

#include<functional>
#include<atomic>
#include<pthread.h>
#include<list>

#include"global.h"

namespace elevencent{
  
  class ThreadPoolRude{
  private:
    class TaskNode{
      friend ThreadPoolRude;
    private:
      std::function<void*(void*)>m_task;
      void*m_arg;
      std::function<void(void*)>m_callback;
    private:
      void doTask(){m_callback(m_task(m_arg));}
      TaskNode(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback):m_task(std::forward<std::function<void*(void*)>>(task)),m_arg(arg),m_callback(std::forward<std::function<void(void*)>>(callback)){}
    };
    static void*thrFunc(void*);
    void createTaskHandler(int num=1);
    TaskNode*popTask();
    void updateThrData(int*thrDatas);
    void pushTaskNode(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback);
    volatile std::atomic<int>m_thrIdle,m_thrBusy,m_curThrNum,m_maxTasks;
    pthread_mutex_t m_rudeTaskMutex;
    pthread_cond_t m_taskCond,m_maxTaskCond;
    pthread_attr_t m_thrAttr;
    std::function<void(ThreadPoolRude*,int*thrDatas)>m_updateThrData;
    std::list<TaskNode*>m_taskList;
  public:
    enum ThrDataIdx{
      ThrDataIdxCached,
      ThrDataIdxMax,

      ThrDataIdxEnd
    };
    ThreadPoolRude();
    ~ThreadPoolRude();
    ThreadPoolRude(std::function<void(ThreadPoolRude*,int*)>&&updateThrData,int maxTasks=1000000);
    bool pushTask(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback,bool noWait=false);
    void setThrDataFunc(std::function<void(ThreadPoolRude*,int*thrDatas)>&&updateThrData);
  };
}

#endif


















