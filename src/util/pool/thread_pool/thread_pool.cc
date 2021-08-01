#include<queue>

#include<assert.h>
#include<cassert>
#include"thread_pool.h"
#include<syslog.h>
#include<unistd.h>

using namespace elevencent;
using namespace std;

#define LEFT_CHILD(p) (((p)->m_left&&(p)->m_left->m_idx==(p)->m_idx*2+1)?(p)->m_left:nullptr)
#define RIGHT_CHILD(p) (((p)->m_right&&(p)->m_right->m_idx==(p)->m_idx*2+2)?(p)->m_right:nullptr)
#define NICE(a) (a)->m_nice
#define MIN_NICE_NODE(a,b,c) (NICE(a)<=NICE(b)?(NICE(a)<=NICE(c)?(a):(c)):(NICE(b)<=NICE(c)?(b):(c)))

#define DFT_MAX_TASKS 32000
#define CHECK_CANCEL(cancelMap,cancelMutex,tid) do{	\
    pthread_mutex_lock(cancelMutex);			\
    if(cancelMap.find(tid)!=cancelMap.end()&&cancelMap[tid]){	\
      pthread_mutex_unlock(cancelMutex);		\
      pthread_exit(0);}					\
    pthread_mutex_unlock(cancelMutex);			\
    }while(0)

#define INIT_COMMON {pthread_mutex_init(&m_taskMutex,0);\
    pthread_mutex_init(&m_maxTaskMutex,0);		\
    pthread_mutex_init(&m_curThrNumMutex,0);\
    pthread_mutex_init(&m_thrIdleMutex,0);  \
    pthread_mutex_init(&m_cancelMutex,0);  \
    pthread_mutex_init(&m_thrBusyMutex,0);  \
    pthread_mutex_init(&m_thrtListMutex,0);\
    pthread_cond_init(&m_taskCond,0);	   \
    pthread_cond_init(&m_maxTaskCond,0);   \
    pthread_cond_init(&m_curThrNumZeroCond,0);   \
    pthread_attr_init(&m_thrAttr);					\
    pthread_attr_setdetachstate(&m_thrAttr,PTHREAD_CREATE_DETACHED);\
    pthread_attr_setscope(&m_thrAttr,PTHREAD_SCOPE_SYSTEM);	    \
    m_head=m_tail=m_tpr=nullptr;		\
    m_thrIdle=m_thrBusy=m_tasks=m_curThrNum=0;\
  }

ThreadPool::ThreadPool(bool niceon):m_niceon(niceon),m_maxTasks(DFT_MAX_TASKS){
  m_updateThrData=[](ThreadPool*pool,short*thrDatas){
    thrDatas[ThrDataIdxCached]=1;
    thrDatas[ThrDataIdxMax]=g_processNum;
  };
  INIT_COMMON;
}

ThreadPool::ThreadPool(function<void(ThreadPool*,short*)>&&updateThrData,short maxTasks,bool niceon):m_niceon(niceon),m_updateThrData(forward<function<void(ThreadPool*,short*)>>(updateThrData)),m_maxTasks(maxTasks){
  if(m_maxTasks<=0)
    m_maxTasks=DFT_MAX_TASKS;
  INIT_COMMON;
}

ThreadPool::~ThreadPool(){
  if(m_niceon){

  }
  
  pthread_mutex_destroy(&m_taskMutex);
  pthread_mutex_destroy(&m_maxTaskMutex);
  pthread_mutex_destroy(&m_curThrNumMutex);
  pthread_mutex_destroy(&m_thrIdleMutex);
  pthread_mutex_destroy(&m_cancelMutex);  
  pthread_mutex_destroy(&m_thrBusyMutex);
  pthread_mutex_destroy(&m_thrtListMutex);
  pthread_cond_destroy(&m_taskCond);
  pthread_cond_destroy(&m_maxTaskCond);
  pthread_cond_destroy(&m_curThrNumZeroCond);  
  pthread_attr_destroy(&m_thrAttr);
}

void ThreadPool::pushTask(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback,short nice){
  short thrDatas[ThrDataIdxEnd];
  updateThrData(thrDatas);  
  pthread_mutex_lock(&m_curThrNumMutex);
  DEBUG_PRETTY_ASSERT(m_curThrNum>=0&&m_thrIdle>=0&&m_thrBusy>=0,"m_curThrNum: "<<m_curThrNum<<", m_thrIdle: "<<m_thrIdle<<", m_thrBusy: "<<m_thrBusy);
  if(m_curThrNum<thrDatas[ThrDataIdxCached]){
    short newThrNum=thrDatas[ThrDataIdxCached]-m_curThrNum;
    m_curThrNum+=newThrNum;
    pthread_mutex_lock(&m_thrIdleMutex);
    m_thrIdle+=newThrNum;
    pthread_mutex_unlock(&m_thrIdleMutex);
    createTaskHandler(newThrNum);
  }
  pthread_mutex_unlock(&m_curThrNumMutex);    
  pthread_mutex_lock(&m_taskMutex);
  while(m_tasks==m_maxTasks){
    pthread_cond_wait(&m_maxTaskCond,&m_taskMutex);
  }
  pushTaskNode(forward<function<void*(void*)>>(task),arg,forward<function<void(void*)>>(callback),nice);
  if(m_tasks==1){
    pthread_cond_signal(&m_taskCond);
  }
  pthread_mutex_unlock(&m_taskMutex);
}

void ThreadPool::pushTaskNode(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback,short nice){
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
  }else{
    m_tpr->m_left=p;
    m_tail->m_right=p;
    p->m_parent=m_tpr;
  }
  p->m_left=m_tail;
  m_tail=p;
 adjust_heap:
  auto pp=p->m_parent;
  if(pp->m_nice<=p->m_nice)return;  
  TaskNode*ppp=pp->m_parent,*pl=p->m_left,*ppl=pp->m_left,*ppr=pp->m_right;
  if(p==ppr){
    m_tpr->m_left=pl->m_parent=p;
    p->m_right=pl->m_right=pp;
    if(pp==m_head)m_head=p;
  }else{
    p->m_left=pp;
    p->m_right=ppr;
    ppr->m_left=p;
    pp->m_left=pl;
    pl->m_right=pp;
    m_tpr=p;
  }
  p->m_idx^=pp->m_idx;pp->m_idx^=p->m_idx;p->m_idx^=pp->m_idx;
  if(ppp)(ppp->m_left==pp?ppp->m_left:ppp->m_right)=p;
  pp->m_parent=p;
  pp->m_right=nullptr;
  p->m_parent=ppp;
  m_tail=pp;
  if(!(pp=ppp)||pp->m_nice<=p->m_nice)return;
  ppp=ppp->m_parent;
  pl=p->m_left;
  TaskNode*pr=p->m_right;
  ppl=pp->m_left;
  ppr=pp->m_right;
  if(m_tpr->m_left==p)m_tpr->m_left=pp;
  if(m_tpr==p)m_tpr=pp;
  pp->m_left=pl;
  pl->m_parent=pp;
  pp->m_right=pr;
  (pr->m_idx==p->m_idx*2+2?pr->m_parent:pr->m_left)=pp;
  if(p==ppl){
    p->m_left=pp;
    p->m_right=ppr; 
    ppr->m_parent=p;
  }else{
    p->m_right=pp;
    p->m_left=ppl;
    ppl->m_parent=p;
  }
  if(ppp)(ppp->m_left==pp?ppp->m_left:ppp->m_right)=p;
  p->m_parent=ppp;
  pp->m_parent=p;
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

ThreadPool::TaskNode*ThreadPool::popTask(){
  if(!m_head)return 0;
  --m_tasks;  
  if(!m_niceon){
    auto task=m_head,tail=m_head->m_left;
    if(m_head=m_head->m_right)m_head->m_left=tail;
    return task;
  }
  TaskNode*task=m_head;
  if(m_head==m_tail){
    m_head=nullptr;
    return task;
  }
  {
    auto hl=m_head->m_left;
    if(hl==m_tail){
      m_head=hl;
      m_head->m_idx=0;
      m_head->m_parent=nullptr;
      return task;
    }
    auto hr=m_head->m_right;
    if(hr==m_tail){
      if(hl->m_nice<=hr->m_nice){
	m_head=hl;
	m_head->m_left=hr;
      }else{
	m_head=hr;
	m_tail=m_head->m_left=m_head->m_right=hl;
      }
      m_tail->m_parent=m_head;
      m_tail->m_idx=1;
      m_head->m_idx=0;
      m_head->m_parent=nullptr;
      return task;
    }
    auto p=MIN_NICE_NODE(m_tail,hl,hr),pp=m_tail;
    {
      auto tl=m_tail->m_left,tp=m_tail->m_parent;

      if(m_tail==tp->m_left){
	m_tpr->m_left=tl->m_parent;	    
      }else{
	tp->m_right=m_tpr;
	m_tpr=m_tpr->m_left=tp;
      }
      m_tail=tl;
      m_tail->m_right=nullptr;
    }
    if(p==pp){
      pp->m_parent=nullptr;
      pp->m_idx=0;
      pp->m_left=hl;
      pp->m_right=hr;
      hl->m_parent=hr->m_parent=m_head=pp;
      return task;
    }
    auto pl=p->m_left,pr=p->m_right;
    pp->m_left=pl;
    pp->m_right=pr;
    if(p==m_tpr)m_tpr=pp;
    if(p==hl){
      if(pl)pl->m_parent=pp;
      (pr->m_idx==p->m_idx*2+2?pr->m_parent:pr->m_left)=pp;
      pp->m_idx=1;
      hl->m_left=pp;
      hl->m_right=hr;
      hr->m_parent=hl;
      if(hr->m_left==hl)hr->m_left=pp;
    }else{	
      if(pl!=hl)pl->m_parent=pp;
      else if(hl->m_right==hr)hl->m_right=pp;
      if(pr)(pr->m_idx==p->m_idx*2+2?pr->m_parent:pr->m_left)=pp;
      if(m_tail==hr)m_tail=pp;
      pp->m_idx=2;
      hr->m_right=pp;
      hr->m_left=hl;
      hl->m_parent=hr;
    }
    pp->m_parent=m_head=p;
    m_head->m_parent=nullptr;
    m_head->m_idx=0;
    auto ppp=m_head;
    TaskNode*PPL,*PPR;
    while((PPL=LEFT_CHILD(pp))&&(PPR=RIGHT_CHILD(pp))&&(p=MIN_NICE_NODE(pp,PPL,PPR))!=pp){
      pp->m_left=pl=p->m_left;
      pp->m_right=pr=p->m_right;
      if(p==PPL){
	if(pl->m_idx==p->m_idx*2+1)pl->m_parent=pp;
	else if(pl->m_right==p)pl->m_right=pp;
	(pr==PPR?pr->m_left:pr->m_parent)=pp;
	if(PPR->m_left==p)PPR->m_left=pp;
	p->m_left=pp;
	p->m_right=PPR;
	PPR->m_parent=p;
      }else{
	if(pl!=PPL)pl->m_parent=pp;
	else if(PPL->m_right==p)PPL->m_right=pp;
	if(pr)(pr->m_idx==p->m_idx*2+2?pr->m_parent:pr->m_left)=pp;
	if(m_tpr->m_left==p)m_tpr->m_left=pp;
	p->m_right=pp;
	p->m_left=PPL;
	PPL->m_parent=p;
      }
      if(p==m_tpr)m_tpr=pp;
      if(p==m_tail)m_tail=pp;
      (ppp->m_left==pp?ppp->m_left:ppp->m_right)=p;
      p->m_parent=ppp;
      pp->m_parent=p;
      p->m_idx^=pp->m_idx;pp->m_idx^=p->m_idx;p->m_idx^=pp->m_idx;
      ppp=p;
    }
    if(PPL&&!PPR&&PPL->m_nice<pp->m_nice){
      PPL->m_right=pp->m_right;
      pp->m_right->m_left=PPL;
      pp->m_right=nullptr;
      pp->m_left=PPL->m_left;
      PPL->m_left->m_right=pp;
      PPL->m_left=pp;
      pp->m_parent=PPL;
      (ppp->m_left==pp?ppp->m_left:ppp->m_right)=PPL;
      PPL->m_parent=ppp;
      PPL->m_idx^=pp->m_idx;pp->m_idx^=PPL->m_idx;PPL->m_idx^=pp->m_idx;
      m_tail=pp;
      m_tpr=PPL;
    }
  }
  return task;
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
  TaskNode*task;
  while((task=m_head)&&num-->0){
    --m_tasks;
    if(m_head==m_tail){
      m_head=nullptr;
      goto consume;
    }
    {
      auto hl=m_head->m_left;
      if(hl==m_tail){
	m_head=hl;
	m_head->m_idx=0;
	m_head->m_parent=nullptr;
	goto consume;
      }
      auto hr=m_head->m_right;
      if(hr==m_tail){
	if(hl->m_nice<=hr->m_nice){
	  m_head=hl;
	  m_head->m_left=hr;
	}else{
	  m_head=hr;
	  m_tail=m_head->m_left=m_head->m_right=hl;
	}
	m_tail->m_parent=m_head;
	m_tail->m_idx=1;
	m_head->m_idx=0;
	m_head->m_parent=nullptr;
	goto consume;
      }
      auto p=MIN_NICE_NODE(m_tail,hl,hr),pp=m_tail;
      {
	auto tl=m_tail->m_left,tp=m_tail->m_parent;

	if(m_tail==tp->m_left){
	  m_tpr->m_left=tl->m_parent;	    
	}else{
	  tp->m_right=m_tpr;
	  m_tpr=m_tpr->m_left=tp;
	}
	m_tail=tl;
	m_tail->m_right=nullptr;
      }
      if(p==pp){
	pp->m_parent=nullptr;
	pp->m_idx=0;
	pp->m_left=hl;
	pp->m_right=hr;
	hl->m_parent=hr->m_parent=m_head=pp;
	goto consume;
      }
      auto pl=p->m_left,pr=p->m_right;
      pp->m_left=pl;
      pp->m_right=pr;
      if(p==m_tpr)m_tpr=pp;
      if(p==hl){
	if(pl)pl->m_parent=pp;
	(pr->m_idx==p->m_idx*2+2?pr->m_parent:pr->m_left)=pp;
	pp->m_idx=1;
	hl->m_left=pp;
	hl->m_right=hr;
	hr->m_parent=hl;
	if(hr->m_left==hl)hr->m_left=pp;
      }else{	
	if(pl!=hl)pl->m_parent=pp;
	else if(hl->m_right==hr)hl->m_right=pp;
	if(pr)(pr->m_idx==p->m_idx*2+2?pr->m_parent:pr->m_left)=pp;
	if(m_tail==hr)m_tail=pp;
	pp->m_idx=2;
	hr->m_right=pp;
	hr->m_left=hl;
	hl->m_parent=hr;
      }
      pp->m_parent=m_head=p;
      m_head->m_parent=nullptr;
      m_head->m_idx=0;
      auto ppp=m_head;
      TaskNode*PPL,*PPR;
      while((PPL=LEFT_CHILD(pp))&&(PPR=RIGHT_CHILD(pp))&&(p=MIN_NICE_NODE(pp,PPL,PPR))!=pp){
	pp->m_left=pl=p->m_left;
	pp->m_right=pr=p->m_right;
	if(p==PPL){
	  if(pl->m_idx==p->m_idx*2+1)pl->m_parent=pp;
	  else if(pl->m_right==p)pl->m_right=pp;
	  (pr==PPR?pr->m_left:pr->m_parent)=pp;
	  if(PPR->m_left==p)PPR->m_left=pp;
	  p->m_left=pp;
	  p->m_right=PPR;
	  PPR->m_parent=p;
	}else{
	  if(pl!=PPL)pl->m_parent=pp;
	  else if(PPL->m_right==p)PPL->m_right=pp;
	  if(pr)(pr->m_idx==p->m_idx*2+2?pr->m_parent:pr->m_left)=pp;
	  if(m_tpr->m_left==p)m_tpr->m_left=pp;
	  p->m_right=pp;
	  p->m_left=PPL;
	  PPL->m_parent=p;
	}
	if(p==m_tpr)m_tpr=pp;
	if(p==m_tail)m_tail=pp;
	(ppp->m_left==pp?ppp->m_left:ppp->m_right)=p;
	p->m_parent=ppp;
	pp->m_parent=p;
	p->m_idx^=pp->m_idx;pp->m_idx^=p->m_idx;p->m_idx^=pp->m_idx;
	ppp=p;
      }
      if(PPL&&!PPR&&PPL->m_nice<pp->m_nice){
	PPL->m_right=pp->m_right;
	pp->m_right->m_left=PPL;
	pp->m_right=nullptr;
	pp->m_left=PPL->m_left;
	PPL->m_left->m_right=pp;
	PPL->m_left=pp;
	pp->m_parent=PPL;
	(ppp->m_left==pp?ppp->m_left:ppp->m_right)=PPL;
	PPL->m_parent=ppp;
	PPL->m_idx^=pp->m_idx;pp->m_idx^=PPL->m_idx;PPL->m_idx^=pp->m_idx;
	m_tail=pp;
	m_tpr=PPL;
      }
    }
  consume:
    task->doTask();
    delete task;
  }
}

void ThreadPool::consumeAllTasks(){
  consumeTask(m_tasks);
}

void ThreadPool::traverseLayer(){
  if(!DEBUG||!m_head)return;
  string layer="";
  pthread_mutex_lock(&m_taskMutex);  
  if(m_niceon){//tree
    auto p=m_head;
    queue<TaskNode*>q;
    q.push(p);
    TaskNode*PR=nullptr,*PL=nullptr;
    while(!q.empty()){
      p=q.front();
      q.pop();
      layer+=("nice: "+to_string(p->m_nice)+", idx: "+to_string(p->m_idx)+"\n");
      if((PL=LEFT_CHILD(p))&&PL->m_parent==p){
	q.push(p->m_left);
	if(NICE(p->m_left)<NICE(p)){
	  cout<<layer<<endl;
	}
	DEBUG_ASSERT(NICE(p->m_left)>=NICE(p),"nice p: "<<NICE(p)<<", idx p: "<<p->m_idx<<"\nnice left: "<<NICE(p->m_left)<<"idx left: "<<p->m_left->m_idx);
      }
      if((PR=RIGHT_CHILD(p))&&PR->m_parent==p){
	q.push(p->m_right);
	if(NICE(p->m_right)<NICE(p)){
	  cout<<layer<<endl;
	}
	DEBUG_ASSERT(NICE(PR)>=NICE(p),"nice p: "<<NICE(p)<<", idx p: "<<p->m_idx<<"\nnice right: "<<NICE(p->m_right)<<"idx right: "<<p->m_right->m_idx);
      }
    }
  }else{//linear
    
  }

  DEBUG_ASSERT(!m_tail||m_tail->m_idx==m_tasks-1,"m_tpr: "<<m_tpr->m_idx<<", m_tail: "<<m_tail->m_idx<<", m_tail->m_parent: "<<m_tail->m_parent->m_idx<<", m_tpr->m_left: "<<m_tpr->m_left->m_idx<<"\n"<<layer);
  if(m_tasks>3){
    if(m_tail==m_tail->m_parent->m_left)
      DEBUG_ASSERT(m_tpr==m_tail->m_parent,"m_tpr: "<<m_tpr->m_idx<<", m_tail: "<<m_tail->m_idx<<", m_tail->m_parent: "<<m_tail->m_parent->m_idx<<", m_tpr->m_left: "<<m_tpr->m_left->m_idx<<"\n"<<layer);
    else
      DEBUG_ASSERT(m_tpr->m_idx==m_tail->m_parent->m_idx+1,"m_tpr: "<<m_tpr->m_idx<<", m_tail: "<<m_tail->m_idx<<", m_tail->m_parent: "<<m_tail->m_parent->m_idx<<", m_tpr->m_left: "<<m_tpr->m_left->m_idx<<"\n"<<layer);
  }
  if(!m_tpr)DEBUG_PRETTY_MSG("!m_tpr");
  if(m_tpr)DEBUG_PRETTY_MSG("m_tpr: idx: "+to_string(m_tpr->m_idx)+", nice: "+to_string(m_tpr->m_nice));
  if(!m_head)DEBUG_PRETTY_MSG("!m_head");
  if(m_head)DEBUG_PRETTY_MSG("m_head: idx: "+to_string(m_head->m_idx)+", nice: "+to_string(m_head->m_nice));
  if(!m_tail)DEBUG_PRETTY_MSG("!m_tail");
  if(m_tail)DEBUG_PRETTY_MSG("m_tail: idx: "+to_string(m_tail->m_idx)+", nice: "+to_string(m_tail->m_nice));
  pthread_mutex_unlock(&m_taskMutex);      
}

void ThreadPool::thrCleanup(void *arg){
  ThreadPool*pool=(ThreadPool*)arg;
  pthread_t tid=pthread_self();
  pthread_mutex_lock(&pool->m_thrtListMutex);
  pool->m_thrtList.remove(tid);
  pthread_mutex_unlock(&pool->m_thrtListMutex);
  pthread_mutex_lock(&pool->m_cancelMutex);
  pool->m_cancelMap.erase(tid);
  pthread_mutex_unlock(&pool->m_cancelMutex);
  if(!--pool->m_curThrNum)
    pthread_cond_signal(&pool->m_curThrNumZeroCond);
  pthread_mutex_unlock(&pool->m_curThrNumMutex);
}

void ThreadPool::thrCleanup1(void*arg){
  ThreadPool*pool=(ThreadPool*)arg;
  pthread_mutex_unlock(&pool->m_taskMutex);
  pthread_mutex_lock(&pool->m_thrIdleMutex);
  --pool->m_thrIdle;
  pthread_mutex_unlock(&pool->m_thrIdleMutex);
  pthread_mutex_lock(&pool->m_curThrNumMutex);
}

void ThreadPool::thrCleanup2(void*arg){
  ThreadPool*pool=(ThreadPool*)arg;
  pthread_mutex_unlock(&pool->m_thrtListMutex);
}

void ThreadPool::thrCleanup3(void*arg){
  ThreadPool*pool=(ThreadPool*)arg;
  pthread_mutex_unlock(&pool->m_taskMutex);  
  --pool->m_thrIdle;
  pthread_mutex_unlock(&pool->m_thrIdleMutex);
}

void ThreadPool::wasteAllTasks(){
  pthread_mutex_lock(&m_taskMutex);
#define CLEAR do{\
    m_head=m_tail=m_tpr=nullptr;\
    m_tasks=0;\
    pthread_mutex_unlock(&m_taskMutex);\
  }while(0)
  if(!m_head){
    CLEAR;
    return;
  }
  if(!m_niceon){
    CLEAR;
    return;
  }
  auto p=m_head;
  queue<TaskNode*>q;
  q.push(p);
  TaskNode*PR=nullptr,*PL=nullptr;
  while(!q.empty()){
    p=q.front();
    q.pop();
    if((PL=LEFT_CHILD(p))&&PL->m_parent==p)
      q.push(p->m_left);
    if((PR=RIGHT_CHILD(p))&&PR->m_parent==p)
      q.push(p->m_right);
    delete p;    
  }
  CLEAR;
}

void ThreadPool::updateThrData(short*thrDatas){
  m_updateThrData(this,thrDatas);
  if(thrDatas[ThrDataIdxCached]<=0)thrDatas[ThrDataIdxCached]=1;
  if(thrDatas[ThrDataIdxMax]<=0)thrDatas[ThrDataIdxMax]=g_processNum;
}

void ThreadPool::test1(){
  DEBUG_PRETTY_ASSERT(m_thrtList.empty()&&m_cancelMap.empty()&&m_curThrNum==0&&m_thrIdle==0&&m_thrBusy==0&&m_tasks==0,"m_curThrNum: "<<m_curThrNum<<", m_thrIdle: "<<m_thrIdle<<", m_thrBusy: "<<m_thrBusy<<", m_tasks: "<<m_tasks);
}

void*ThreadPool::thrFunc(void*arg){
  if(!arg)return 0;
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,0);
  ThreadPool*pool=(ThreadPool*)arg;
  pthread_cleanup_push(thrCleanup,arg);
  short thrDatas[ThrDataIdxEnd];
  pthread_t tid=pthread_self();
  while(1){
    DEBUG_PRETTY_ASSERT(pool->m_curThrNum>0&&pool->m_thrIdle>=0&&pool->m_thrBusy>=0,"m_curThrNum: "<<pool->m_curThrNum<<", m_thrIdle: "<<pool->m_thrIdle<<", m_thrBusy: "<<pool->m_thrBusy);
    pool->updateThrData(thrDatas);
    pthread_mutex_lock(&pool->m_curThrNumMutex);
    short maxD=thrDatas[ThrDataIdxMax]-pool->m_curThrNum;
    if(maxD<0){
      pthread_mutex_lock(&pool->m_thrIdleMutex);
      --pool->m_thrIdle;
      pthread_mutex_unlock(&pool->m_thrIdleMutex);
      pthread_exit(0);
    }
    pthread_mutex_lock(&pool->m_taskMutex);
    pthread_mutex_lock(&pool->m_thrIdleMutex);    
    short cachedD=thrDatas[ThrDataIdxCached]-pool->m_curThrNum;
    short taskD=pool->m_tasks-pool->m_thrIdle;
    if(taskD<0&&cachedD<0){
      pthread_mutex_unlock(&pool->m_taskMutex);
      --pool->m_thrIdle;
      pthread_mutex_unlock(&pool->m_thrIdleMutex);
      pthread_exit(0);
    }
    short d=min(maxD,max(cachedD,taskD));
    if(d>0){
      pthread_cleanup_push(thrCleanup3,arg);      
      pool->createTaskHandler(d);
      pthread_cleanup_pop(0);      
      pool->m_curThrNum+=d;
      pool->m_thrIdle+=d;      
    }
    pthread_mutex_unlock(&pool->m_curThrNumMutex);
    pthread_mutex_unlock(&pool->m_thrIdleMutex);
    while(pool->m_tasks==0){
      pthread_cleanup_push(thrCleanup1,arg);          
      CHECK_CANCEL(pool->m_cancelMap,&pool->m_cancelMutex,tid);
      pthread_cond_wait(&pool->m_taskCond,&pool->m_taskMutex);
      pthread_cleanup_pop(0);                  
    }
    auto task=pool->popTask();
    if(pool->m_tasks==pool->m_maxTasks-1)
      pthread_cond_signal(&pool->m_maxTaskCond);
    pthread_mutex_unlock(&pool->m_taskMutex);
    pthread_mutex_lock(&pool->m_thrIdleMutex);
    --pool->m_thrIdle;
    pthread_mutex_unlock(&pool->m_thrIdleMutex);
    pthread_mutex_lock(&pool->m_thrBusyMutex);
    ++pool->m_thrBusy;
    pthread_mutex_unlock(&pool->m_thrBusyMutex);    
    if(task){
      task->doTask();
      delete task;
    }
    pthread_mutex_lock(&pool->m_thrIdleMutex);
    ++pool->m_thrIdle;
    pthread_mutex_unlock(&pool->m_thrIdleMutex);    
    pthread_mutex_lock(&pool->m_thrBusyMutex);    
    --pool->m_thrBusy;
    pthread_mutex_unlock(&pool->m_thrBusyMutex);    
  }
  pthread_exit(0);
  pthread_cleanup_pop(0);    
}

void*ThreadPool::clearAllThrsFunc(void*arg){
  ThreadPool*pool=(ThreadPool*)arg;
  pthread_mutex_lock(&pool->m_thrtListMutex);
  pthread_mutex_lock(&pool->m_cancelMutex);
  for(auto iter=pool->m_thrtList.begin();iter!=pool->m_thrtList.end();++iter){
    pool->m_cancelMap[*iter]=true;
  }
  pthread_mutex_unlock(&pool->m_cancelMutex);  
  pthread_mutex_unlock(&pool->m_thrtListMutex);
  pthread_mutex_lock(&pool->m_curThrNumMutex);    
  while(pool->m_curThrNum){
    pthread_mutex_lock(&pool->m_taskMutex);
    pthread_cond_broadcast(&pool->m_taskCond);
    pthread_mutex_unlock(&pool->m_taskMutex);
    pthread_cond_wait(&pool->m_curThrNumZeroCond,&pool->m_curThrNumMutex);    
  }
  DEBUG_PRETTY_ASSERT(pool->m_curThrNum==0,"pool->m_curThrNum: "<<pool->m_curThrNum);  
  pthread_mutex_unlock(&pool->m_curThrNumMutex);
  pthread_mutex_lock(&pool->m_taskMutex);
  pool->consumeAllTasks();
  DEBUG_PRETTY_ASSERT(pool->m_tasks==0&&pool->m_curThrNum==0&&pool->m_thrIdle==0&&pool->m_thrBusy==0,"pool->m_tasks: "<<pool->m_tasks);
  pthread_mutex_unlock(&pool->m_taskMutex);  
  pool->m_clearAllThrsCb(pool->m_clearAllThrsCbArg);
  pthread_exit(0);
}

void ThreadPool::clearAllThrs(bool wasteTasks){
  clearAllThrs([](void*){},nullptr,wasteTasks);
}

void ThreadPool::clearAllThrs(function<void(void*arg)>&&cb,void*arg,bool wasteTasks){
  m_clearAllThrsCb=forward<function<void(void*arg)>>(cb);
  m_clearAllThrsCbArg=arg;
  if(wasteTasks){
    wasteAllTasks();
  }
  pthread_t tid;
  if(pthread_create(&tid,&m_thrAttr,clearAllThrsFunc,this))
    DEBUG_PRETTY_ASSERT(1,"pthread_create error");
}

void ThreadPool::createTaskHandler(short num){
  pthread_t tid;
  pthread_cleanup_push(thrCleanup2,this);  
  pthread_mutex_lock(&m_thrtListMutex);
  CHECK_CANCEL(m_cancelMap,&m_cancelMutex,pthread_self());
  while(num-->0){
    if(pthread_create(&tid,&m_thrAttr,thrFunc,this))    
      DEBUG_PRETTY_ASSERT(1,"pthread_create error");
    else
      m_thrtList.push_back(tid);
  }
  pthread_mutex_unlock(&m_thrtListMutex);
  pthread_cleanup_pop(0);
}

void ThreadPool::setThrDataFunc(function<void(ThreadPool*,short*thrDatas)>&&updateThrData){
  m_updateThrData=forward<function<void(ThreadPool*,short*)>>(updateThrData);
}
