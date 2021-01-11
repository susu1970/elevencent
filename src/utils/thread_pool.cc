#include<queue>

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
  if(!m_tail->m_parent){
    m_head->m_left=m_head->m_right=m_tail=p;
    p->m_parent=m_head;
    goto adjust_heap;
  }
  if(m_tail==m_tail->m_parent->m_left){//left-child
    m_tpr=m_tail->m_parent->m_right;
    m_tail->m_parent->m_right=m_tail->m_right=p;
    p->m_parent=m_tail->m_parent;
    p->m_left=m_tail;
  }else{//right-child
    m_tpr->m_left=p;
    m_tail->m_right=p;
    p->m_left=m_tail;
    p->m_parent=m_tpr;
  }
  m_tail=p;
 adjust_heap:
  auto pp=p->m_parent;
  if(pp->m_nice<=p->m_nice)return;  
  TaskNode*ppp=pp->m_parent,*pl=nullptr,*pr=nullptr,*ppl=pp->m_left,*ppr=RIGHT_CHILD(pp);
  m_tail=pp;
  do{
    if(pp==m_head)m_head=p;
    p->m_idx^=pp->m_idx;pp->m_idx^=p->m_idx;p->m_idx^=pp->m_idx;
    p->m_parent=ppp;
    if(p==ppl){
      p->m_left=pp;
      p->m_right=pp->m_right;
      if(ppr)ppr->m_parent=p;
    }else{
      p->m_right=pp;
      (p->m_left=ppl)&&(ppl->m_parent=p);
    }
    if(ppp)(pp==ppp->m_left?ppp->m_left:ppp->m_right)=p;
    pp->m_parent=p;
    (pp->m_left=pl)&&(pl->m_parent=pp);
    (pp->m_right=pr)&&(pr->m_parent=pp);
    (pp=ppp)&&(ppp=ppp->m_parent);
    pl=p->m_left;
    pr=p->m_right;
    if(pp){
      ppl=pp->m_left;
      ppr=pp->m_right;
    }
  }while(pp&&pp->m_nice>p->m_nice);
  
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
  if(m_niceon){//tree
    auto p=m_head;
    queue<TaskNode*>q;
    q.push(p);
    while(!q.empty()){
      p=q.front();
      q.pop();
      DEBUG_PRETTY_MSG("idx: "+to_string(p->m_idx)+", nice: "+to_string(p->m_nice));
      if(LEFT_CHILD(p))q.push(p->m_left);
      if(RIGHT_CHILD(p))q.push(p->m_right);
    }
    if(!m_tpr)DEBUG_PRETTY_MSG("!m_tpr");
    if(m_tpr)DEBUG_PRETTY_MSG("m_tpr: idx: "+to_string(m_tpr->m_idx)+", nice: "+to_string(m_tpr->m_nice));
    if(!m_head)DEBUG_PRETTY_MSG("!m_head");
    if(m_head)DEBUG_PRETTY_MSG("m_head: idx: "+to_string(m_head->m_idx)+", nice: "+to_string(m_head->m_nice));
    if(!m_tail)DEBUG_PRETTY_MSG("!m_tail");
    if(m_tail)DEBUG_PRETTY_MSG("m_tail: idx: "+to_string(m_tail->m_idx)+", nice: "+to_string(m_tail->m_nice));
  }else{//linear
    
  }
}

void ThreadPool::TaskNode::doTask(){
  m_callback(m_task(m_arg));
}

ThreadPool::TaskNode::TaskNode(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback,short nice,short idx):m_nice(nice),m_task(forward<function<void*(void*)>>(task)),m_arg(arg),m_callback(forward<function<void(void*)>>(callback)),m_left(nullptr),m_right(nullptr),m_parent(nullptr),m_idx(idx){
  
}





