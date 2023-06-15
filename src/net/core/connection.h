#ifndef ELEVENCENT_CONNECTION_H_
#define ELEVENCENT_CONNECTION_H_

#include"socket.h"
#include"epoll.h"
#include"process_interface.h"
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<functional>
#include<sys/time.h>
#include"resource_limit.hpp"

namespace elevencent{
  class Connection{
  public:
    enum ERRNO{
      ERRNO_OK=0,
      ERRNO_AGAIN=1,
      ERRNO_CLOSE=1<<1,
      ERRNO_RLIMIT_QOS=1<<2,
    };
  public:
    void*ctx;
    std::function<void(Connection*)>onDestroy;
    ProcessInterface*process;
    struct sockaddr sa;
    socklen_t salen;
    int fd;
    ResourceLimit::Qos*inQos=0,*outQos=0;
    ResourceLimit::Cpu*inCpu=0,*outCpu=0;    
    ssize_t read(void*buf,size_t count,int*err=0);
    ssize_t write(const void*buf,size_t count,int*err=0);
    ssize_t recv(void *buf,size_t len,int flags);
    Connection(const int fd,const struct sockaddr&sa,const socklen_t salen,ProcessInterface*process,void*ctx,std::function<void(Connection*)>&&onDestroy);
    static void*handleIn(void*arg);
    static void handleInCb(void*arg);
    static void*handleOut(void*arg);
    static void handleOutCb(void*arg);
    ~Connection();
  };
  class TcpConnection:public Connection{
  public:
    TcpConnection(const int fd,const struct sockaddr&sa,const socklen_t salen,ProcessInterface*process,void*ctx,std::function<void(Connection*)>&&onDestroy);    
    ~TcpConnection();
  };
}

#endif
