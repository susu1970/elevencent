#include<queue>

#include<assert.h>
#include<cassert>
#include"thread_pool.h"
#include<syslog.h>


using namespace elevencent;
using namespace std;

#define LEFT_CHILD(p) (((p)->m_left&&(p)->m_left->m_idx==(p)->m_idx*2+1)?(p)->m_left:nullptr)
#define RIGHT_CHILD(p) (((p)->m_right&&(p)->m_right->m_idx==(p)->m_idx*2+2)?(p)->m_right:nullptr)
#define NICE(a) (a)->m_nice
#define MIN_NICE_NODE(a,b,c) (NICE(a)<=NICE(b)?(NICE(a)<=NICE(c)?(a):(c)):(NICE(b)<=NICE(c)?(b):(c)))

ThreadPool::ThreadPool(bool niceon,short maxTasks):m_niceon(niceon),m_idle(0),m_busy(0),m_tasks(0),m_head(nullptr),m_tail(nullptr),m_tpr(nullptr),mk_maxTasks(maxTasks){
  ASSERT(maxTasks>0,"");

  pthread_mutex_init(&m_headMutex,0);
  pthread_cond_init(&m_taskCond,0);
  pthread_cond_init(&m_maxTaskCond,0);
}

//TODO!
ThreadPool::~ThreadPool(){
  if(m_niceon){
    
  }
  pthread_mutex_destroy(&m_headMutex);
  pthread_cond_destroy(&m_taskCond);
  pthread_cond_destroy(&m_maxTaskCond);
}

void ThreadPool::pushTask(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback,short nice){
  pthread_mutex_lock(&m_taskMutex);
  while(m_tasks==mk_maxTasks){
    pthread_cond_wait(&m_maxTaskCond,&m_taskMutex);
  }
  pushTaskNode(forward<function<void*(void*)>>(task),arg,forward<function<void(void*)>>(callback),nice);
  pthread_cond_signal(&m_taskCond);
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
#if DEBUG
    traverseLayer();
    //cout<<"after:"<<endl;
#endif
    //    delete task;
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
  if(!DEBUG||!m_head)return;
  string layer="";
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

  return;
  if(!m_tpr)DEBUG_PRETTY_MSG("!m_tpr");
  if(m_tpr)DEBUG_PRETTY_MSG("m_tpr: idx: "+to_string(m_tpr->m_idx)+", nice: "+to_string(m_tpr->m_nice));
  if(!m_head)DEBUG_PRETTY_MSG("!m_head");
  if(m_head)DEBUG_PRETTY_MSG("m_head: idx: "+to_string(m_head->m_idx)+", nice: "+to_string(m_head->m_nice));
  if(!m_tail)DEBUG_PRETTY_MSG("!m_tail");
  if(m_tail)DEBUG_PRETTY_MSG("m_tail: idx: "+to_string(m_tail->m_idx)+", nice: "+to_string(m_tail->m_nice));
}

short ThreadPool::tasks(){
  return m_tasks;
}

void ThreadPool::TaskNode::doTask(){
  m_callback(m_task(m_arg));
}

void ThreadPool::createTaskHandler(short num){
  while(num-->0){
    pthread_t tid;
    if(pthread_create(&tid,0,[&this](void*arg)->void*{
      while(1){
	pthread_mutex_lock(&m_taskMutex);
	while(m_task<=0){
	  pthread_cond_wait(&m_taskCond,&m_taskMutex);
	}
	auto task=this->popTask();
	if(m_tasks==mk_maxTasks-1)
	  pthread_cond_signal(&m_maxTaskCond);
	pthread_mutex_unlock(&m_taskMutex);
	if(task){
	  task->doTask();
	  delete task;
	}
      }
      return nullptr;
    },0)){
      SYSLOG(LOG_ERR,"pthread_create != 0");
    }else{ 
      m_tidList.push_back(tid);
    }
  }
}

ThreadPool::TaskNode::TaskNode(std::function<void*(void*)>&&task,void*arg,std::function<void(void*)>&&callback,short nice,short idx):m_nice(nice),m_task(forward<function<void*(void*)>>(task)),m_arg(arg),m_callback(forward<function<void(void*)>>(callback)),m_left(nullptr),m_right(nullptr),m_parent(nullptr),m_idx(idx){
  
}


