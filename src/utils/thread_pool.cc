#include<queue>

#include<assert.h>
#include<cassert>
#include"thread_pool.h"

using namespace elevencent;
using namespace std;

#define LEFT_CHILD(p) (((p)->m_left&&(p)->m_left->m_idx==(p)->m_idx*2+1)?(p)->m_left:nullptr)
#define RIGHT_CHILD(p) (((p)->m_right&&(p)->m_right->m_idx==(p)->m_idx*2+2)?(p)->m_right:nullptr)

ThreadPool::ThreadPool(bool niceon):m_niceon(niceon),m_idle(0),m_busy(0),m_tasks(0),m_head(nullptr),m_tail(nullptr),m_tpr(nullptr){
  
}

void ThreadPool::pushTask(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback,short nice){
  TaskNode*p=new TaskNode(forward<function<void*(void*)>>(task),arg,forward<function<void(void*)>>(callback),nice,m_tasks++);
  DEBUG_PRETTY_MSG("idx: "+to_string(p->m_idx)+"\tnice: "+to_string(p->m_nice));
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
  if(m_tail==m_head){
    if(p->m_nice<m_head->m_nice){
      m_head=p;
    }else{
      m_tail=p;
    }
    m_head->m_left=m_head->m_right=m_tail;
    m_tail->m_parent=m_head;
    m_head->m_idx=0;
    m_tail->m_idx=1;
    return;
  }
  if(m_tail==m_tail->m_parent->m_left){
    m_tpr=m_tail->m_parent->m_right;
    m_tail->m_parent->m_right=m_tail->m_right=p;
    p->m_parent=m_tail->m_parent;
    p->m_left=m_tail;
  }else{
    m_tpr->m_left=p;
    m_tail->m_right=p;
    p->m_left=m_tail;
    p->m_parent=m_tpr;
  }
  m_tail=p;
 adjust_heap:
  auto pp=p->m_parent;
  if(pp->m_nice<=p->m_nice)return;  
  TaskNode*ppp=pp->m_parent,*pl=p->m_left,*pr=nullptr,*ppl=pp->m_left,*ppr=pp->m_right;
  if(p==ppl){
    pl->m_right=pp;
    pp->m_left=pl;
    ppr->m_left=p;
    p->m_right=ppr;
    p->m_left=pp;
  }else{
    ppl->m_right=pp;
    ppl->m_parent=p;
    p->m_right=pp;
  }
  pp->m_parent=p;
  pp->m_right=nullptr;  
  if(ppp)(ppp->m_left==pp?ppp->m_left:ppp->m_right)=p;
  p->m_parent=ppp;
  m_tail=pp;
  p->m_idx^=pp->m_idx;pp->m_idx^=p->m_idx;p->m_idx^=pp->m_idx;
  if(p->m_idx==0)m_head=p;
  if(!(pp=ppp)||pp->m_nice<=p->m_nice)return;
  ppp=ppp->m_parent;
  pl=p->m_left;
  pr=p->m_right;
  ppl=pp->m_left;
  ppr=pp->m_right;
  pp->m_left=pl;
  pl->m_parent=pp;
  pp->m_right=pr;
  {
    TaskNode*PR=RIGHT_CHILD(p);
    if(PR)PR->m_parent=pp;
  }
  if(p==ppl){
    p->m_left=pp;
    p->m_right=ppr;
    ppr->m_parent=p;
  }else{
    p->m_right=pp;
    p->m_left=ppl;
    ppl->m_parent=p;
  }
  pp->m_parent=p;
  if(ppp)(ppp->m_left==pp?ppp->m_left:ppp->m_right)=p;
  p->m_parent=ppp;
  p->m_idx^=pp->m_idx;pp->m_idx^=p->m_idx;p->m_idx^=pp->m_idx;
  while((pp=ppp)&&(pp->m_nice>p->m_nice)){
    ppp=ppp->m_parent;
    pl=p->m_left;
    pr=p->m_right;
    ppl=pp->m_left;
    ppr=pp->m_right;
    pp->m_left=pl;
    pl->m_parent=pp;
    pp->m_right=pr;
    pr->m_parent=pp;
    if(p==ppl){
      p->m_left=pp;
      p->m_right=ppr;
      ppr->m_parent=p;
    }else{
      p->m_right=pp;
      p->m_left=ppl;
      ppl->m_parent=p;
    }
    pp->m_parent=p;
    if(ppp)(ppp->m_left==pp?ppp->m_left:ppp->m_right)=p;
    p->m_parent=ppp;
    p->m_idx^=pp->m_idx;pp->m_idx^=p->m_idx;p->m_idx^=pp->m_idx;
  }
  if(p->m_idx==0)m_head=p;  
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
  while(m_head&&num-->0){
    auto task=m_head;
  adjust_heap:
     
  }
}

void ThreadPool::run(){
  
}

void ThreadPool::setNiceon(bool niceon){
  m_niceon=niceon;
}

bool ThreadPool::niceon(){
  return m_niceon;
}

void ThreadPool::traverseLayer(){
  if(!DEBUG)return;
  string layer="";
  if(m_niceon){//tree
    auto p=m_head;
    queue<TaskNode*>q;
    q.push(p);
    while(!q.empty()){
      p=q.front();
      q.pop();
      DEBUG_PRETTY_MSG("idx: "+to_string(p->m_idx)+", nice: "+to_string(p->m_nice));
      if(LEFT_CHILD(p)){
	q.push(p->m_left);	
      }
      if(RIGHT_CHILD(p)){
	q.push(p->m_right);
      }
    }
  }else{//linear
    
  }
  if(!m_tpr)DEBUG_PRETTY_MSG("!m_tpr");
  if(m_tpr)DEBUG_PRETTY_MSG("m_tpr: idx: "+to_string(m_tpr->m_idx)+", nice: "+to_string(m_tpr->m_nice));
  if(!m_head)DEBUG_PRETTY_MSG("!m_head");
  if(m_head)DEBUG_PRETTY_MSG("m_head: idx: "+to_string(m_head->m_idx)+", nice: "+to_string(m_head->m_nice));
  if(!m_tail)DEBUG_PRETTY_MSG("!m_tail");
  if(m_tail)DEBUG_PRETTY_MSG("m_tail: idx: "+to_string(m_tail->m_idx)+", nice: "+to_string(m_tail->m_nice));
}

void ThreadPool::TaskNode::doTask(){
  m_callback(m_task(m_arg));
}

ThreadPool::TaskNode::TaskNode(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback,short nice,short idx):m_nice(nice),m_task(forward<function<void*(void*)>>(task)),m_arg(arg),m_callback(forward<function<void(void*)>>(callback)),m_left(nullptr),m_right(nullptr),m_parent(nullptr),m_idx(idx){
  
}


