#ifndef ELEVENCENT_CONNECTION_H_
#define ELEVENCENT_CONNECTION_H_

#include"socket.h"
#include"epoll.h"
#include"process_interface.h"
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<functional>

namespace elevencent{
  class Connection{
  public:
    void*ctx;
    std::function<void(Connection*)>onDestroy;
    ProcessInterface*process;
    struct sockaddr sa;
    socklen_t salen;
    int fd;
    ssize_t read(void*buf,size_t count);
    ssize_t write(const void*buf,size_t count);
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
