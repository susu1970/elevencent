#ifndef _ELEVENCENT_SPINLOCK_HPP_
#define _ELEVENCENT_SPINLOCK_HPP_
#include"global.h"
#include<sched.h>
namespace elevencent{
  class SpinLock{
    volatile int8_t m_lock=0;
  public:
    SpinLock(){
      m_lock=0;
    }
    void lock(){
      while(!CAS_BOOL(&m_lock,0,1))
	sched_yield();
    }
    void unlock(){
      while(!CAS_BOOL(&m_lock,1,0))
	sched_yield();
    }
  };
}

#endif
