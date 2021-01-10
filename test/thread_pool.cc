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
  },-1);
  goto msg;
  pool->pushTask([](void*arg)->void*{
    return nullptr;
  },nullptr,[](void*ret){
  },-3);
  pool->pushTask([](void*arg)->void*{
    return nullptr;
  },nullptr,[](void*ret){
  },1);

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
 msg:
  DEBUG_PRETTY_MSG("");
  pool->traverseLayer();
}


















