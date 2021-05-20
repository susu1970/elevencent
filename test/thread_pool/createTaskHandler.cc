#include<unistd.h>
#include"test.h"
#include"thread_pool.h"
#include<pthread.h>

class MyTestCase:public TestCase{
public:
  virtual void runTest(){
    
  }
};
#define TASKS 10000
int main(int argc,char**argv){
  ThreadPool*pool=new ThreadPool([](ThreadPool*pool,short*thrDatas){
    thrDatas[ThrDataIdxCached]=1;
    thrDatas[ThrDataIdxMax]=8;    
  },20000);
  int finishedTask=0;
  for(int i=0;i<TASKS;++i){
    pool->pushTask([i](void*arg)->void*{
      //      cout<<"task("<<i<<") working"<<endl;
      return 0;
    },0,[i](void*arg){
      //      cout<<"task("<<i<<") finished"<<endl;

    },i);    
  }
  pause();
}
