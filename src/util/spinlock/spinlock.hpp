#ifndef _ELEVENCENT_SPINLOCK_HPP_
#define _ELEVENCENT_SPINLOCK_HPP_
#include"global.h"
#include<pthread.h>

namespace elevencent{
  class SpinLock{
    volatile int8_t m_lock=0;
    pthread_mutex_t m_mutex;
  public:
    SpinLock(){
      m_lock=0;
      pthread_mutex_init(&m_mutex,0);
      DEBUG_MSG("init");
    }
    void lock(){
      pthread_mutex_lock(&m_mutex);
      //      while(!CAS_BOOL(&m_lock,0,1));
    }
    void unlock(){
      pthread_mutex_unlock(&m_mutex);      
      //      while(!CAS_BOOL(&m_lock,1,0));
    }
    ~SpinLock(){
      pthread_mutex_destroy(&m_mutex);      
    }
  };
}

#endif
