#include"thread_pool.h"
#include"spinlock.hpp"
#include"test.h"

int main(int argc,char**argv){
  SpinLock lock;
  ThreadPool thrPool;
  thrPool.setThrDataFunc([](ThreadPool*tp,short*A){
    A[ThrDataIdxCached]=4;
    A[ThrDataIdxMax]=40;      
  });
  int i=0;
  while(1)
  thrPool.pushTask([&](void*arg)->void*{
    //    lock.lock();
    int oldi=i;
    ++i;
    DEBUG_ASSERT(oldi+1==i,"ERROR");
    //    lock.unlock();
    return 0;
  },0,[&](void*){
    //    lock.lock();
    int oldi=i;
    --i;
    DEBUG_ASSERT(oldi-1==i,"ERROR");
    //    lock.unlock();    
  },TaskNice::TaskNiceDft,false);
  while(1)sleep(1);
}
