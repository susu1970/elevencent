#include"thread_pool.h"

using namespace elevencent;
using namespace std;

ThreadPool::ThreadPool(){
  m_mutex=pthread_mutex_init(&m_mutex,nullptr);
  m_cond=pthread_cond_init(&m_cond,nullptr);
}

void ThreadPool::delThread(short num){
  
}

void ThreadPool::addThread(short num){
  while(num-->0){
    thread_t tid;
    if(pthread_create(&tid,nullptr,thr,nullptr)){
      DEBUG_MSG("error");
      return;
    }
    m_idlethrs.push_back(tid);
  }
}

void*thr(void*arg){
  DEBUG_MSG("");
  while(1){
    
  }
}








