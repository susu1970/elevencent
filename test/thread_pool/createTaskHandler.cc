#include<unistd.h>
#include"test.h"
#include"thread_pool.h"
#include<pthread.h>

class MyTestCase:public TestCase{
public:
  virtual void runTest(){
    
  }
};
#define TASKS 323
int main(int argc,char**argv){
  srand(time(0));
  while(1){
    int i=rand()%TASKS;
    ThreadPool*pool=new ThreadPool();
    volatile bool finished=false;
    while(i-->0){
      pool->pushTask([i](void*arg)->void*{
	//	for(int j=0;j<10000;++j)
	//	  for(int k=0;k<1000;++k);
	//      sleep(1);
	//	DEBUG_PRETTY_MSG("task("<<i<<") start");
	return 0;
      },0,[i](void*arg){
	//	for(int j=0;j<10000;++j)
	//	  for(int k=0;k<1000;++k);      
	//      sleep(1);
	//	DEBUG_PRETTY_MSG("task("<<i<<") end");	
      },i);    
      pool->setThrDataFunc([](ThreadPool*pool,short*thrDatas){
	thrDatas[ThrDataIdxCached]=rand()%8-2;
	thrDatas[ThrDataIdxMax]=rand()%46-3;
      });
    }
    pool->clearAllThrs([&finished](void*arg){
      ThreadPool*p=(ThreadPool*)arg;
      DEBUG_MSG("");
      p->test1();
      delete p;
      finished=true;
    },pool,true);
    while(!finished);
    finished=false;
  }
  pause();
}
