#include "thread_pool.h"
#include<pthread.h>
#include<syslog.h>

using namespace elevencent;
using namespace std;

ThreadPoolNode::ThreadPoolNode(function<void*(void*)>&&task,void*arg,function<void(void*)>&&callback,short nice)
  :m_task(move(task)),m_callback(move(callback)),m_nice(nice),m_left(nullptr),m_right(nullptr),m_parent(nullptr){
  
}

ThreadPool::ThreadPool(bool enableNice,unsigned short minIdle,unsigned short maxBusy)
  :m_niceEnabled(enableNice),m_minIdle(minIdle),m_maxBusy(maxBusy),m_root(nullptr){
  pthread_mutex_init(&m_mutex,0);
  
}

void ThreadPool::run(){

}

void ThreadPool::pause(){
  
}

ThreadPool::~ThreadPool(){
  pthread_mutex_destroy(&m_mutex);
}










