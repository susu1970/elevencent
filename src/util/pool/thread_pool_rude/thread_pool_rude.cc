#include<assert.h>
#include<cassert>
#include"thread_pool_rude.h"
using namespace elevencent;
using namespace std;
#define DFT_MAX_TASKS 32000
#define INIT_COMMON do{\
    pthread_mutex_init(&m_rudeTaskMutex,0);				\
    pthread_cond_init(&m_taskCond,0);					\
    pthread_cond_init(&m_maxTaskCond,0);				\
    pthread_attr_init(&m_thrAttr);					\
    pthread_attr_setdetachstate(&m_thrAttr,PTHREAD_CREATE_DETACHED);	\
    pthread_attr_setscope(&m_thrAttr,PTHREAD_SCOPE_SYSTEM);		\
    m_thrIdle=m_thrBusy=m_curThrNum=0;				\
  }while(0)

ThreadPoolRude::ThreadPoolRude():m_maxTasks(DFT_MAX_TASKS){
  m_updateThrData=[](ThreadPoolRude*pool,int*thrDatas){
    thrDatas[ThrDataIdxCached]=1;
    thrDatas[ThrDataIdxMax]=g_processNum;
  };
  INIT_COMMON;
}
ThreadPoolRude::ThreadPoolRude(function<void(ThreadPoolRude*,int*)>&&updateThrData,int maxTasks):m_updateThrData(forward<function<void(ThreadPoolRude*,int*)>>(updateThrData)),m_maxTasks(maxTasks){
  if(m_maxTasks<=0)
    m_maxTasks=DFT_MAX_TASKS;
  INIT_COMMON;
}
ThreadPoolRude::~ThreadPoolRude(){
  pthread_mutex_destroy(&m_rudeTaskMutex);
  pthread_cond_destroy(&m_taskCond);
  pthread_cond_destroy(&m_maxTaskCond);
  pthread_attr_destroy(&m_thrAttr);
}
bool ThreadPoolRude::pushTask(function<void*(void*)>&&task,void*arg,function<void(void*)>&&callback,bool noWait){
  int thrDatas[ThrDataIdxEnd];
  updateThrData(thrDatas);  
  pthread_mutex_lock(&m_rudeTaskMutex);
  if(m_curThrNum<thrDatas[ThrDataIdxCached]){
    int newThrNum=thrDatas[ThrDataIdxCached]-m_curThrNum;
    m_curThrNum+=newThrNum;
    m_thrIdle+=newThrNum;
    createTaskHandler(newThrNum);
  }
  while(m_taskList.size()==m_maxTasks){
    if(noWait){
      pthread_mutex_unlock(&m_rudeTaskMutex);
      return false;
    }
    pthread_cond_wait(&m_maxTaskCond,&m_rudeTaskMutex);
  }
  pushTaskNode(forward<function<void*(void*)>>(task),arg,forward<function<void(void*)>>(callback));
  if(m_taskList.size()==1)
    pthread_cond_signal(&m_taskCond);
  pthread_mutex_unlock(&m_rudeTaskMutex);
  return true;
}
void ThreadPoolRude::pushTaskNode(function<void*(void*)>&&task,void*arg,function<void(void*)>&&callback){
  m_taskList.push_back(new TaskNode(forward<function<void*(void*)>>(task),arg,forward<function<void(void*)>>(callback)));
  return;
}
ThreadPoolRude::TaskNode*ThreadPoolRude::popTask(){
  if(m_taskList.empty())
    return 0;
  auto task=m_taskList.front();
  m_taskList.pop_front();
  return task;
}
void*ThreadPoolRude::thrFunc(void*arg){
  ThreadPoolRude*pool=(ThreadPoolRude*)arg;
  int thrDatas[ThrDataIdxEnd];
  while(1){
    pool->updateThrData(thrDatas);
    pthread_mutex_lock(&pool->m_rudeTaskMutex);
    int maxD=thrDatas[ThrDataIdxMax]-pool->m_curThrNum;
    if(maxD<0){
      --pool->m_thrIdle;
      --pool->m_curThrNum;
      pthread_mutex_unlock(&pool->m_rudeTaskMutex);      
      pthread_exit(0);
    }
    int cachedD=thrDatas[ThrDataIdxCached]-pool->m_curThrNum;
    int taskD=pool->m_taskList.size()-pool->m_thrIdle;
    int d=min(maxD,max(cachedD,taskD));
    if(d<0){
      --pool->m_thrIdle;
      --pool->m_curThrNum;
      pthread_mutex_unlock(&pool->m_rudeTaskMutex);
      pthread_exit(0);
    }
    pool->createTaskHandler(d);
    pool->m_curThrNum+=d;
    pool->m_thrIdle+=d;      
    while(pool->m_taskList.size()==0)
      pthread_cond_wait(&pool->m_taskCond,&pool->m_rudeTaskMutex);
    auto task=pool->popTask();
    if(pool->m_taskList.size()==pool->m_maxTasks-1)
      pthread_cond_signal(&pool->m_maxTaskCond);
    --pool->m_thrIdle;
    ++pool->m_thrBusy;
    pthread_mutex_unlock(&pool->m_rudeTaskMutex);    
    task->doTask();
    delete task;
    pthread_mutex_lock(&pool->m_rudeTaskMutex);
    ++pool->m_thrIdle;   
    --pool->m_thrBusy;
    pthread_mutex_unlock(&pool->m_rudeTaskMutex);
  }
}
void ThreadPoolRude::createTaskHandler(int num){
  pthread_t tid;
  while(num-->0)
    if(pthread_create(&tid,&m_thrAttr,thrFunc,this))    
      DEBUG_PRETTY_ASSERT(1,"pthread_create error");
}
void ThreadPoolRude::setThrDataFunc(function<void(ThreadPoolRude*,int*thrDatas)>&&updateThrData){
  m_updateThrData=forward<function<void(ThreadPoolRude*,int*thrDatas)>>(updateThrData);
}
void ThreadPoolRude::updateThrData(int*thrDatas){
  m_updateThrData(this,thrDatas);
  if(thrDatas[ThrDataIdxCached]<=0)thrDatas[ThrDataIdxCached]=1;
  if(thrDatas[ThrDataIdxMax]<thrDatas[ThrDataIdxCached])thrDatas[ThrDataIdxMax]=thrDatas[ThrDataIdxCached];
}
