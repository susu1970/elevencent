#include<unistd.h>
#include"test.h"
#include"thread_pool.h"
#include<pthread.h>

class MyTestCase:public TestCase{
public:
  virtual void runTest(){
    
  }
};
#define TASKS 32300
int main(int argc,char**argv){
  ThreadPool*pool=new ThreadPool();
  srand(time(0));
  while(1){
    for(int i=0;i<TASKS;++i){
      pool->pushTask([i](void*arg)->void*{
	sleep(1);
	return 0;
      },0,[i](void*arg){
	sleep(1);
      },i);    
      pool->setThrDataFunc([](ThreadPool*pool,short*thrDatas){
	thrDatas[ThrDataIdxCached]=rand()%8-2;
	thrDatas[ThrDataIdxMax]=rand()%46-3;
      });
    }
    wasteAllTasks();
  }
  pause();
}
