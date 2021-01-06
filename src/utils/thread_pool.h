#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include<list>
#include<functional>
#include<pthread.h>

#include"global.h"

namespace elevencent{
  class ThreadPool{
    friend std::ostream&operator<<(ostream&,ThreadPool&);
  private:
    void delThread(short num=1);
    void addThread(short num=2);
    void*thr(void*arg);
  private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
    bool m_niceon;
    short m_idle,m_busy,m_maxbusy,m_minidle,m_maxidle;
    std::list<pthread_t>m_idlethrs,m_busythrs;
  public:
    ThreadPool(short minidle=2,short maxidle=8,short maxbusy=17,bool niceon=false);
  };
  std::ostream&operator<<(std::ostream&o,ThreadPool&);
}

#endif

















