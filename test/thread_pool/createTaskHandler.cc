#include<unistd.h>
#include"test.h"
#include"thread_pool.h"

class MyTestCase:public TestCase{
public:
  virtual void runTest(){
    
  }
};
#define TASKS 10
int main(int argc,char**argv){
  ThreadPool*pool=new ThreadPool([](ThreadPool*pool,short*thrDatas){
    thrDatas[ThrDataIdxCached]=2;
    thrDatas[ThrDataIdxMax]=3;    
  });
  int finishedTask=0;
  for(int i=0;i<TASKS;++i){
    pool->pushTask([i](void*arg)->void*{
      //      cout<<"task("<<i<<") working"<<endl;
      return 0;
    },0,[i](void*arg){
      cout<<"task("<<i<<") finished"<<endl;
    },i);    
  }
  pause();
}
