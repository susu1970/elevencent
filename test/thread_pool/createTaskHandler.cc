#include<unistd.h>
#include"test.h"
#include"thread_pool.h"
#include<pthread.h>

class MyTestCase:public TestCase{
public:
  virtual void runTest(){
    
  }
};
#define TASKS 3230
int main(int argc,char**argv){
  srand(time(0));
  while(1){
    int i=rand()%TASKS;
    ThreadPool*pool=new ThreadPool([](ThreadPool*pool,short*A){
      A[ThrDataIdxCached]=5;
      A[ThrDataIdxMax]=16;
    },32000);
    volatile bool finished=false;
    i=TASKS;
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
      /*
      pool->setThrDataFunc([](ThreadPool*pool,short*thrDatas){
	thrDatas[ThrDataIdxCached]=rand()%8-2;
	thrDatas[ThrDataIdxMax]=rand()%46-3;
      });
      */
    }
    sleep(4);
    pool->clearAllThrs([&finished](void*arg){
      ThreadPool*p=(ThreadPool*)arg;
      DEBUG_MSG("clearAllThrs finished");
      p->test1();
      delete p;
      finished=true;
    },pool,true);
    while(!finished);
    finished=false;
    pause();
  }

}
