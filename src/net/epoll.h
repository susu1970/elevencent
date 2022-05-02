#ifndef ELEVENCENT_EPOLL_H_
#define ELEVENCENT_EPOLL_H_

#include<sys/epoll.h>
#include"global.h"

namespace elevencent{
  class Epoll{
  public:
    int epfd;
    Epoll();
    ~Epoll();
    int ctl(int op,int fd,struct epoll_event*event);
    int wait(struct epoll_event*event,int maxevents,int timeout);
  };
}

#endif
