#ifndef ELEVENCENT_CONNECTION_H_
#define ELEVENCENT_CONNECTION_H_

#include"socket.h"
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include"epoll.h"

namespace elevencent{
  class Connection{
  public:
    struct sockaddr sa;
    socklen_t salen;
    int fd;
    class Buf{
    private:
      char*buf;
      size_t len;
      size_t maxlen;
    public:
      Buf(size_t maxlen=10*1024);
      bool append(const void*src,size_t n);
      size_t popFront(void*dst,size_t n);
      size_t size();
      void*peekBuf()const;
      ~Buf();
    };
    Connection(const int fd,const struct sockaddr&sa,const socklen_t salen);
    static void*handleIn(void*arg){return arg;}
    static void handleInCb(void*arg){}
    static void*handleOut(void*arg){return arg;}
    static void handleOutCb(void*arg){}
    int getnameinfo(char*host,socklen_t hostlen,char*serv,socklen_t servlen,int flags);
  };
  class TcpConnection:public Connection{
    Buf inBuf,outBuf;
    Epoll*ep;
  public:
    std::function<void*(void*arg)>m_handleIn;
    std::function<void(void*arg)>m_handleInCb;
    std::function<void*(void*arg)>m_handleOut;
    std::function<void(void*arg)>m_handleOutCb;    
    TcpConnection(const int fd,const struct sockaddr&sa,const socklen_t salen,Epoll*ep);
    static void*handleIn(void*arg);
    static void handleInCb(void*arg);
    static void*handleOut(void*arg);
    static void handleOutCb(void*arg);
    ssize_t write(size_t count);
    ssize_t read(size_t count);
    ~TcpConnection();
  };
}

#endif
