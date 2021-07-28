#include<unistd.h>
#include"test.h"
#include"thread_pool.h"
#include<pthread.h>

class MyTestCase:public TestCase{
public:
  virtual void runTest(){
    
  }
};
#define TASKS 30001
int main(int argc,char**argv){
  srand(time(0));
  unsigned char old=1;
  unsigned long long loop_times=0;
  ThreadPool*g_pool=new ThreadPool();
  while(1){
    ++old;
    ++loop_times;
    volatile bool finished=false;      
    for(int i=-10;i<11;++i){
      for(int j=-10;j<11;++j){
	for(int k=-10;k<11;++k){
	  ThreadPool*pool=new ThreadPool([&](ThreadPool*pool,short*A){
	    A[ThrDataIdxCached]=i;
	    A[ThrDataIdxMax]=j;
	  },k);
	  for(int n=-10;n<11;++n){
	    pool->pushTask([=](void*arg)->void*{
	      int top=-rand()%3200+3200;
	      while(top++!=10000);
	      return 0;
	    },0,[=](void*arg){
	      int top=-rand()%3200+3200;
	      while(top++!=10000);
	    },k);    
	  }
	  pool->clearAllThrs([&](void*arg){
	    ThreadPool*p=(ThreadPool*)arg;
	    DEBUG_MSG("clearAllThrs finished"<<"\nloop_timers: "<<loop_times<<endl);
	    p->test1();
	    finished=true;
	    delete p;
	  },pool,old%2);
	  while(!finished) ;
	  finished=false;
	}
      }
    }
    for(int i=-10;i<11;++i){
      for(int j=-10;j<11;++j){
	for(int k=-10;k<11;++k){
	  ThreadPool*pool=new ThreadPool([&i,&j](ThreadPool*pool,short*A){
	    A[ThrDataIdxCached]=i;
	    A[ThrDataIdxMax]=j;
	  },k);
	  for(int n=-10;n<11;++n){
	    pool->pushTask([&](void*arg)->void*{
	      int top=-rand()%3200+3200;
	      while(top++!=10000);
	      return 0;
	    },0,[&i,&j](void*arg){
	      int top=-rand()%3200+3200;
	      while(top++!=10000);
	    },k);    
	  }
	  pool->setThrDataFunc([&](ThreadPool*pool,short*thrDatas){
	    thrDatas[ThrDataIdxCached]=j;
	    thrDatas[ThrDataIdxMax]=k;
	  });
	  pool->clearAllThrs([&](void*arg){
	    ThreadPool*p=(ThreadPool*)arg;
	    DEBUG_MSG("clearAllThrs finished"<<"\nloop_timers: "<<loop_times<<endl);
	    p->test1();
	    finished=true;
	    delete p;
	  },pool,old%2);
	  while(!finished) ;
	  finished=false;
	}
      }
    }
    for(int i=-10;i<11;++i){
      for(int j=-10;j<11;++j){
	for(int k=-10;k<11;++k){
	  for(int n=-10;n<11;++n){
	    g_pool->pushTask([&](void*arg)->void*{
	      int top=-rand()%3200+3200;
	      while(top++!=10000);
	      return 0;
	    },0,[&](void*arg){
	      int top=-rand()%3200+3200;
	      while(top++!=10000);
	    },k);    
	  }
	  g_pool->clearAllThrs([&](void*arg){
	    ThreadPool*p=(ThreadPool*)arg;
	    DEBUG_MSG("g_pool clearAllThrs finished"<<"\nloop_timers: "<<loop_times<<endl);
	    p->test1();
	    finished=true;
	  },g_pool,old%2);
	  while(!finished) ;
	  finished=false;
	}
      }
    }
    for(int i=-10;i<11;++i){
      for(int j=-10;j<11;++j){
	for(int k=-10;k<11;++k){
	  for(int n=-10;n<11;++n){
	    g_pool->pushTask([&](void*arg)->void*{
	      int top=-rand()%3200+3200;
	      while(top++!=10000);
	      return 0;
	    },0,[&i,&j](void*arg){
	      int top=-rand()%3200+3200;
	      while(top++!=10000);
	    },k);    
	  }
	  g_pool->setThrDataFunc([&](ThreadPool*pool,short*thrDatas){
	    thrDatas[ThrDataIdxCached]=k;
	    thrDatas[ThrDataIdxMax]=i;
	  });
	  g_pool->clearAllThrs([&](void*arg){
	    ThreadPool*p=(ThreadPool*)arg;
	    DEBUG_MSG("g_pool clearAllThrs finished"<<"\nloop_timers: "<<loop_times<<endl);
	    p->test1();
	    finished=true;
	  },g_pool,old%2);
	  while(!finished) ;
	  finished=false;
	}
      }
    }
    for(int i=rand()%-42+22;i<42;++i){
      for(int j=rand()%-42+22;j<42;++j){
	int k=TASKS;
	ThreadPool*pool=new ThreadPool([&i,&j](ThreadPool*pool,short*A){
	  A[ThrDataIdxCached]=i;
	  A[ThrDataIdxMax]=j;
	},k);
	for(int n=-rand()%-32001;n<32011;++n){
	  pool->pushTask([&](void*arg)->void*{
	    int top=-rand()%3200+3200;
	    while(top++!=10000);
	    return 0;
	  },0,[&i,&j](void*arg){
	    int top=-rand()%3200+3200;
	    while(top++!=10000);
	  },k);    
	}
	pool->clearAllThrs([&](void*arg){
	  ThreadPool*p=(ThreadPool*)arg;
	  DEBUG_MSG("clearAllThrs finished"<<"\nloop_timers: "<<loop_times<<endl);
	  p->test1();
	  finished=true;
	  delete p;
	},pool,old%2);
	while(!finished) ;
	finished=false;
      }
    }
    for(int i=rand()%-42+22;i<42;++i){
      for(int j=rand()%-42+22;j<42;++j){
	int k=TASKS;
	ThreadPool*pool=new ThreadPool([&i,&j](ThreadPool*pool,short*A){
	  A[ThrDataIdxCached]=i;
	  A[ThrDataIdxMax]=j;
	},k);
	for(int n=-rand()%-32001;n<32011;++n){
	  pool->pushTask([&](void*arg)->void*{
	    int top=-rand()%3200+3200;
	    while(top++!=10000);
	    return 0;
	  },0,[&i,&j](void*arg){
	    int top=-rand()%3200+3200;
	    while(top++!=10000);
	  },k);    
	}
	pool->setThrDataFunc([&](ThreadPool*pool,short*thrDatas){
	  thrDatas[ThrDataIdxCached]=rand()%2*2-2;
	  thrDatas[ThrDataIdxCached]=rand()%8*2-8;
	});
	pool->clearAllThrs([&](void*arg){
	  ThreadPool*p=(ThreadPool*)arg;
	  DEBUG_MSG("clearAllThrs finished"<<"\nloop_timers: "<<loop_times<<endl);
	  p->test1();
	  finished=true;
	  delete p;
	},pool,old%2);
	while(!finished) ;
	finished=false;
      }
    }

    for(int i=rand()%-42+22;i<42;++i){
      for(int j=rand()%-42+22;j<42;++j){
	int k=TASKS;
	for(int n=-rand()%-32001;n<32011;++n){
	  g_pool->pushTask([&](void*arg)->void*{
	    int top=-rand()%3200+3200;
	    while(top++!=10000);
	    return 0;
	  },0,[&i,&j](void*arg){
	    int top=-rand()%3200+3200;
	    while(top++!=10000);
	  },k);    
	}
	g_pool->clearAllThrs([&](void*arg){
	  ThreadPool*p=(ThreadPool*)arg;
	  DEBUG_MSG("g_pool clearAllThrs finished"<<"\nloop_timers: "<<loop_times<<endl);
	  p->test1();
	  finished=true;
	},g_pool,old%2);
	while(!finished) ;
	finished=false;
      }
    }
    for(int i=rand()%-42+22;i<42;++i){
      for(int j=rand()%-42+22;j<42;++j){
	int k=TASKS;
	for(int n=-rand()%-32001;n<32011;++n){
	  g_pool->pushTask([&](void*arg)->void*{
	    int top=-rand()%3200+3200;
	    while(top++!=10000);
	    return 0;
	  },0,[&i,&j](void*arg){
	    int top=-rand()%3200+3200;
	    while(top++!=10000);
	  },k);    
	}
	g_pool->setThrDataFunc([&](ThreadPool*pool,short*thrDatas){
	  thrDatas[ThrDataIdxCached]=rand()%2*2-2;
	  thrDatas[ThrDataIdxCached]=rand()%8*2-8;
	});
	g_pool->clearAllThrs([&](void*arg){
	  ThreadPool*p=(ThreadPool*)arg;
	  DEBUG_MSG("g_pool clearAllThrs finished"<<"\nloop_timers: "<<loop_times<<endl);
	  p->test1();
	  finished=true;
	},g_pool,old%2);
	while(!finished) ;
	finished=false;
      }
    }
  }
}
