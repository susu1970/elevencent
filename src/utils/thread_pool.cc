#include<syslog.h>

#include"thread_pool.h"

using namespace elevencent;
using namespace std;

ThreadPool::ThreadPool(bool niceon):m_niceon(niceon),m_idle(0),m_busy(0),m_tasks(0),m_head(nullptr),m_tail(nullptr),m_tps(nullptr){
  
}

void ThreadPool::pushTask(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback,short nice){
  TaskNode*p=new TaskNode(forward<function<void*(void*)>>(task),arg,forward<function<void(void*)>>(callback),nice,m_tasks++);
  if(!m_niceon){
    if(!m_head){      
      m_head->m_left=m_head=p;
      return;
    }
    m_head->m_left=m_head->m_left->m_right=p;
    return;
  }
  if(!m_head){
    m_tail=m_head=p;
    return;
  }
  if(!m_tail->m_parent){
    m_head->m_left=m_head->m_right=m_tail=p;
    p->m_parent=m_head;
    goto adjust_heap;
  }
  if(m_tail->m_idx==m_tail->m_parent->m_idx*2+1){//left-child
    m_tps=m_tail->m_parent->m_right;
    m_tail->m_parent->m_right=m_tail->m_right=p;
    p->m_parent=m_tail->m_parent;
    p->m_left=m_tail;
  }else{//right-child
    m_tps->m_left=p;
    m_tail->m_right=p;
    p->m_left=m_tail;
    p->m_parent=m_tps;
  }
  m_tail=p;
 adjust_heap:
  DEBUG_PRETTY_MSG("idx: "+to_string(p->m_idx)+" parent: "+to_string(p->m_parent->m_idx)+" nice: "+to_string(p->m_nice));
  while(p->m_parent&&p->m_parent->m_nice>p->m_nice){
    auto pp=p->m_parent,ppp=pp->m_parent;
    auto ppleft=pp->m_left,ppright=pp->m_right;
    if(m_tail->m_parent==pp){
      m_tail=pp;
      DEBUG_MSG("");
    }
    DEBUG_MSG("tail idx: "+to_string(m_tail->m_idx)+", pp idx: "+to_string(pp->m_idx));
    pp->m_left=p->m_left;
    pp->m_right=p->m_right;
    if(pp->m_left)pp->m_left->m_parent=pp;
    if(pp->m_right)pp->m_right->m_parent=pp;
    if(p==ppleft){
      p->m_left=pp;
      p->m_right=ppright;
    }else{
      p->m_right=pp;
      p->m_left=ppleft;
    }
    if(p->m_left)p->m_left->m_parent=p;
    if(p->m_right)p->m_right->m_parent=p;
    if(ppp){
      ppp->m_left==pp?ppp->m_left=p:ppp->m_right=p;
    }
    p->m_parent=ppp;
    p->m_idx^=pp->m_idx;
    pp->m_idx^=p->m_idx;
    p->m_idx^=pp->m_idx;    
    p=ppp;
  }
}

void ThreadPool::consumeTask(short num){
  if(!m_niceon){
    while(m_head&&num-->0){
      auto task=m_head,tail=m_head->m_left;
      if(m_head=m_head->m_right)m_head->m_left=tail;
      task->doTask();
      --m_tasks;
    }
    return;
  }
  #if not DEBUG
  auto p=m_head;
  while(p){
    DEBUG_PRETTY_MSG("idx: "+to_string(p->m_idx));
    p=p->m_left;
  }
  #endif
}

void ThreadPool::run(){
  
}

void ThreadPool::setNiceon(bool niceon){
  m_niceon=niceon;
}

bool ThreadPool::niceon(){
  return m_niceon;
}

void ThreadPool::TaskNode::doTask(){
  m_callback(m_task(m_arg));
}

ThreadPool::TaskNode::TaskNode(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback,short nice,short idx):m_nice(nice),m_task(forward<function<void*(void*)>>(task)),m_arg(arg),m_callback(forward<function<void(void*)>>(callback)),m_left(nullptr),m_right(nullptr),m_parent(nullptr),m_idx(idx){
  
}





