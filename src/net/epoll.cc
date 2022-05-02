#include"epoll.h"
using namespace std;
using namespace elevencent;
Epoll::Epoll(){
  if((epfd=epoll_create1(0))==-1){
    DEBUG_MSG("error");
    return;
  }
}
int Epoll::ctl(int op,int fd,struct epoll_event*event){
  int ret=epoll_ctl(epfd,op,fd,event);
  if(ret==-1)
    DEBUG_MSG("error");
  return ret;
}
int Epoll::wait(struct epoll_event*event,int maxevents,int timeout){
  int ret=epoll_wait(epfd,event,maxevents,timeout);
  if(ret==-1)
    DEBUG_MSG("error");
  return ret;
}
Epoll::~Epoll(){
  if(epfd>=0)
    close(epfd);
}
