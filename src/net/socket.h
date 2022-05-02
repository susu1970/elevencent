#ifndef ELEVENCENT_SOCKET_H_
#define ELEVENCENT_SOCKET_H_

#include"global.h"
#include<sys/types.h>
#include<sys/socket.h>
#include<stdint.h>

namespace elevencent{
  class Socket{
  public:
    int fd;
    Socket(int fd=-1);
    Socket(int domain,int type,int protocol=0);
    Socket(int type,uint16_t port);    
    int bind(const struct sockaddr*addr,socklen_t addrlen);
    int listen(int backlog);
    int accept(struct sockaddr*addr,socklen_t*addrlen);
    int accept4(struct sockaddr*addr,socklen_t*addrlen,int flags);
    int fcntl(int cmd,...);
    ~Socket();
  };
}

#endif
