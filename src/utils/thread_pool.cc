#include"thread_pool.h"

using namespace elevencent;
using namespace std;
/*
ThreadPool::ThreadPool(){
  m_cond=pthread_cond_init(&m_cond,NULL);
  m_cond_mutex=pthread_mutex_init(&m_mutex,NULL);
}

void ThreadPool::delThread(short num){
  
}

void ThreadPool::addThread(short num){
  while(num-->0){
    thread_t tid;
    if(pthread_create(&tid,NULL,thr,NULL)){
      DEBUG_MSG("error");
      return;
    }
    m_idlethrs.push_back(tid);
  }
}

void*ThreadPool::thr(void*arg){
  DEBUG_MSG("");
  thread_cleanup_push([](void*arg)->void*{
    DEBUG_MSG("");
    return nullptr;
  },nullptr);
  while(1){
    pthread_testcancel();
    pthread_mutex_lock(&m_cond_mutex);
    pthread_cond_wait(&m_cond,&m_cond_mutex);
    auto task=popTask();
    pthread_cond_unlock(&m_mutex);
    if(popTask){
      popTask->doTask();
    }
  }
}

ThreadPoolNode*ThreadPool::popTask(){
  
}

ThreadPool::~ThreadPool(){
  
}
*/

void ThreadPoolNode::doTask(){
  if(m_task){
    if(m_callback)((*m_callback)((*m_task)(m_arg)));
    else (*m_task)(m_arg);
  }
  //  m_task&&m_callback?(*m_callback)((*m_task)(m_arg)):(*m_task)(m_arg);
}

ThreadPoolNode::ThreadPoolNode(std::function<void*(void*)>*task,void*arg,std::function<void(void*)>*callback,TaskNice nice):m_task(task),m_arg(arg),m_callback(callback),m_nice(nice){
  
}

ThreadPoolNode::ThreadPoolNode():m_nice(TaskNice::dft),m_task(nullptr),m_callback(nullptr),m_left(nullptr),m_right(nullptr),m_parent(nullptr),m_arg(nullptr){
  
}
