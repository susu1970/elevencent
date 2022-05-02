#include"socket.h"
#include<unistd.h>
#include<fcntl.h>
#include<stdarg.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
using namespace std;
using namespace elevencent;
Socket::Socket(int domain,int type,int protocol){
  if(unlikely((fd=socket(domain,type,protocol))==-1)){
    DEBUG_MSG("error");
    return;
  }
  int opt=1;
  if(unlikely(setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))==-1))
    DEBUG_MSG("error");
}
Socket::Socket(int type,uint16_t port){
  if(unlikely((fd=socket(AF_INET,type,0))==-1)){
    DEBUG_MSG("error");
    return;
  }
  int opt=1;
  if(unlikely(setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))==-1)){
    DEBUG_MSG("error");
    return;
  }
  struct sockaddr_in addr;
  memset(&addr,0,sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_addr.s_addr=htonl(INADDR_ANY);
  addr.sin_port=htons(port);
  if(unlikely(bind((struct sockaddr*)&addr,sizeof(addr))==-1)){
    DEBUG_MSG("error");
    return;
  }
  if(unlikely((type|SOCK_STREAM)&&::listen(fd,512)==-1)){
    DEBUG_MSG("error");
    return;
  }
}
int Socket::fcntl(int cmd,...){
  va_list args;
  va_start(args,cmd);
  int ret=::fcntl(fd,cmd,args);
  va_end(args);
  return ret;
}
Socket::Socket(int argfd):fd(argfd){}
int Socket::bind(const struct sockaddr*addr,socklen_t addrlen){
  int ret=::bind(fd,addr,addrlen);
  if(unlikely(ret==-1))
    DEBUG_MSG("error");
  return ret;
}
int Socket::listen(int backlog){
  int ret=::listen(fd,backlog);
  if(unlikely(ret==-1))
    DEBUG_MSG("error");
  return ret;
}
int Socket::accept(struct sockaddr*addr,socklen_t*addrlen){
  return ::accept(fd,addr,addrlen);
}
int Socket::accept4(struct sockaddr*addr,socklen_t*addrlen,int flags){
  return ::accept4(fd,addr,addrlen,flags);
}
Socket::~Socket(){
  if(fd>=0)
    close(fd);
}
    
