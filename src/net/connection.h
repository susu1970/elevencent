#ifndef ELEVENCENT_CONNECTION_H_
#define ELEVENCENT_CONNECTION_H_

#include"socket.h"
#include<sys/socket.h>
#include<arpa/inet.h>

namespace elevencent{
  class Connection{
  public:
    struct sockaddr sa;
    socklen_t salen;
    int fd;
    //    Socket sk;
    Connection(const int fd,const struct sockaddr&sa,const socklen_t salen);
    static void*handle(void*arg);
    static void handleCb(void*arg);
    int getnameinfo(char*host,socklen_t hostlen,char*serv,socklen_t servlen,int flags);
  };
  class TcpConnection:public Connection{
  public:
    TcpConnection(const int fd,const struct sockaddr&sa,const socklen_t salen);
    static void*handle(void*arg);
    static void handleCb(void*arg);    
    ~TcpConnection();
  };
}

#endif
