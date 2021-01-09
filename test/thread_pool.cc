#include"test.h"

#include"thread_pool.h"

class MyTestCase:public TestCase{
public:
  virtual void runTest(){
    
  }
};

int main(){
  ThreadPool*pool=new ThreadPool(true);
  pool->pushTask([](void*arg)->void*{
    return nullptr;
  },nullptr,[](void*ret){
  },8);    
  pool->pushTask([](void*arg)->void*{
    return nullptr;
  },nullptr,[](void*ret){
  },3);    
  pool->pushTask([](void*arg)->void*{
    return nullptr;
  },nullptr,[](void*ret){
  },9);    
  pool->pushTask([](void*arg)->void*{
    return nullptr;
  },nullptr,[](void*ret){
  },7);
  pool->pushTask([](void*arg)->void*{
    return nullptr;
  },nullptr,[](void*ret){
  },13);    
  pool->pushTask([](void*arg)->void*{
    return nullptr;
  },nullptr,[](void*ret){
  },2);
  pool->pushTask([](void*arg)->void*{
    return nullptr;
  },nullptr,[](void*ret){
  },9);    
  pool->pushTask([](void*arg)->void*{
    return nullptr;
  },nullptr,[](void*ret){
  },4);    

  pool->consumeTask();
  pool->consumeTask(3);
  pool->consumeTask(9);
}


















