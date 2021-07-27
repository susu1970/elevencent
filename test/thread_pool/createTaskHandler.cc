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
    volatile bool finished=false;      
    for(int i=-100;i<111;++i){
      for(int j=-100;j<111;++j){
	for(int k=-100;k<111;++k){
	  ThreadPool*pool=new ThreadPool([&](ThreadPool*pool,short*A){
	    A[ThrDataIdxCached]=i;
	    A[ThrDataIdxMax]=j;
	  },k);
	  for(int n=-100;n<111;++n){
	    pool->pushTask([=](void*arg)->void*{
	      int top=-rand()%3200+3200;
	      for(int i1=i;i1<top;++i1)
		for(int j1=i1;j1<top;++j1);
	      return 0;
	    },0,[=](void*arg){
	      int top=-rand()%3200+3200;
	      for(int i1=i;i1<top;++i1)
		for(int j1=i1;j1<top;++j1);
	    },k);    
	  }
	  pool->clearAllThrs([&](void*arg){
	    ThreadPool*p=(ThreadPool*)arg;
	    DEBUG_MSG("clearAllThrs finished"<<"\nloop_timers: "<<loop_times<<endl);
	    p->test1();
	    finished=true;
	    delete p;
	  },pool,old%2);
	  while(!finished);
	  finished=false;
	}
      }
    }
    for(int i=-100;i<111;++i){
      for(int j=-100;j<111;++j){
	for(int k=-100;k<111;++k){
	  ThreadPool*pool=new ThreadPool([&i,&j](ThreadPool*pool,short*A){
	    A[ThrDataIdxCached]=i;
	    A[ThrDataIdxMax]=j;
	  },k);
	  for(int n=-100;n<111;++n){
	    pool->pushTask([&](void*arg)->void*{
	      int top=-rand()%3200+3200;
	      for(int i1=i;i1<top;++i1)
		for(int j1=i1;j1<top;++j1);
	      return 0;
	    },0,[&i,&j](void*arg){
	      int top=-rand()%3200+3200;
	      for(int i1=i;i1<top;++i1)
		for(int j1=i1;j1<top;++j1);
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
	  while(!finished);
	  finished=false;
	}
      }
    }
    for(int i=-100;i<111;++i){
      for(int j=-100;j<111;++j){
	for(int k=-100;k<111;++k){
	  for(int n=-100;n<111;++n){
	    g_pool->pushTask([&](void*arg)->void*{
	      int top=-rand()%3200+3200;
	      for(int i1=i;i1<top;++i1)
		for(int j1=i1;j1<top;++j1);
	      return 0;
	    },0,[&](void*arg){
	      int top=-rand()%3200+3200;
	      for(int i1=i;i1<top;++i1)
		for(int j1=i1;j1<top;++j1);
	    },k);    
	  }
	  g_pool->clearAllThrs([&](void*arg){
	    ThreadPool*p=(ThreadPool*)arg;
	    DEBUG_MSG("g_pool clearAllThrs finished"<<"\nloop_timers: "<<loop_times<<endl);
	    p->test1();
	    finished=true;
	  },g_pool,old%2);
	  while(!finished);
	  finished=false;
	}
      }
    }
    for(int i=-100;i<111;++i){
      for(int j=-100;j<111;++j){
	for(int k=-100;k<111;++k){
	  for(int n=-100;n<111;++n){
	    g_pool->pushTask([&](void*arg)->void*{
	      int top=-rand()%3200+3200;
	      for(int i1=i;i1<top;++i1)
		for(int j1=i1;j1<top;++j1);
	      return 0;
	    },0,[&i,&j](void*arg){
	      int top=-rand()%3200+3200;
	      for(int i1=i;i1<top;++i1)
		for(int j1=i1;j1<top;++j1);
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
	  while(!finished);
	  finished=false;
	}
      }
    }

    for(int i=rand()%-32000;i<32000;++i){
      for(int j=rand()%-32001;j<32004;++j){
	for(int k=rand()%-32009;k<32003;++k){
	  ThreadPool*pool=new ThreadPool([&i,&j](ThreadPool*pool,short*A){
	    A[ThrDataIdxCached]=i;
	    A[ThrDataIdxMax]=j;
	  },k);
	  for(int n=-rand()%-32001;n<32111;++n){
	    pool->pushTask([&](void*arg)->void*{
	      int top=-rand()%3200+3200;
	      for(int i1=i;i1<top;++i1)
		for(int j1=i1;j1<top;++j1);
	      return 0;
	    },0,[&i,&j](void*arg){
	      int top=-rand()%3200+3200;
	      for(int i1=i;i1<top;++i1)
		for(int j1=i1;j1<top;++j1);
	    },k);    
	  }
	  pool->clearAllThrs([&](void*arg){
	    ThreadPool*p=(ThreadPool*)arg;
	    DEBUG_MSG("clearAllThrs finished"<<"\nloop_timers: "<<loop_times<<endl);
	    p->test1();
	    finished=true;
	    delete p;
	  },pool,old%2);
	  while(!finished);
	  finished=false;
	}
      }
    }
    for(int i=-100;i<111;++i){
      for(int j=-100;j<111;++j){
	for(int k=-100;k<111;++k){
	  ThreadPool*pool=new ThreadPool([&i,&j](ThreadPool*pool,short*A){
	    A[ThrDataIdxCached]=i;
	    A[ThrDataIdxMax]=j;
	  },k);
	  for(int n=-rand()%-32001;n<32111;++n){
	    pool->pushTask([&](void*arg)->void*{
	      int top=-rand()%3200+3200;
	      for(int i1=i;i1<top;++i1)
		for(int j1=i1;j1<top;++j1);
	      return 0;
	    },0,[&i,&j](void*arg){
	      int top=-rand()%3200+3200;
	      for(int i1=i;i1<top;++i1)
		for(int j1=i1;j1<top;++j1);
	    },k);    
	  }
	  pool->setThrDataFunc([&](ThreadPool*pool,short*thrDatas){
	    thrDatas[ThrDataIdxCached]=rand()%19*2-19;
	    thrDatas[ThrDataIdxCached]=rand()%21*2-21;
	  });
	  pool->clearAllThrs([&](void*arg){
	    ThreadPool*p=(ThreadPool*)arg;
	    DEBUG_MSG("clearAllThrs finished"<<"\nloop_timers: "<<loop_times<<endl);
	    p->test1();
	    finished=true;
	    delete p;
	  },pool,old%2);
	  while(!finished);
	  finished=false;
	}
      }
    }

    for(int i=rand()%-32000;i<32000;++i){
      for(int j=rand()%-32001;j<32004;++j){
	for(int k=rand()%-32009;k<32003;++k){
	  for(int n=-rand()%-32001;n<32111;++n){
	    g_pool->pushTask([&](void*arg)->void*{
	      int top=-rand()%3200+3200;
	      for(int i1=i;i1<top;++i1)
		for(int j1=i1;j1<top;++j1);
	      return 0;
	    },0,[&i,&j](void*arg){
	      int top=-rand()%3200+3200;
	      for(int i1=i;i1<top;++i1)
		for(int j1=i1;j1<top;++j1);
	    },k);    
	  }
	  g_pool->clearAllThrs([&](void*arg){
	    ThreadPool*p=(ThreadPool*)arg;
	    DEBUG_MSG("g_pool clearAllThrs finished"<<"\nloop_timers: "<<loop_times<<endl);
	    p->test1();
	    finished=true;
	  },g_pool,old%2);
	  while(!finished);
	  finished=false;
	}
      }
    }
    for(int i=-100;i<111;++i){
      for(int j=-100;j<111;++j){
	for(int k=-100;k<111;++k){
	  for(int n=-rand()%-32001;n<32111;++n){
	    g_pool->pushTask([&](void*arg)->void*{
	      int top=-rand()%3200+3200;
	      for(int i1=i;i1<top;++i1)
		for(int j1=i1;j1<top;++j1);
	      return 0;
	    },0,[&i,&j](void*arg){
	      int top=-rand()%3200+3200;
	      for(int i1=i;i1<top;++i1)
		for(int j1=i1;j1<top;++j1);
	    },k);    
	  }
	  g_pool->setThrDataFunc([&](ThreadPool*pool,short*thrDatas){
	    thrDatas[ThrDataIdxCached]=rand()%19*2-19;
	    thrDatas[ThrDataIdxCached]=rand()%21*2-21;
	  });
	  g_pool->clearAllThrs([&](void*arg){
	    ThreadPool*p=(ThreadPool*)arg;
	    DEBUG_MSG("g_pool clearAllThrs finished"<<"\nloop_timers: "<<loop_times<<endl);
	    p->test1();
	    finished=true;
	  },g_pool,old%2);
	  while(!finished);
	  finished=false;
	}
      }
    }
  }
  ++loop_times;
}
