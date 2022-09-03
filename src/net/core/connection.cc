#include"connection.h"
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<unistd.h>
using namespace std;
using namespace elevencent;
Connection::Connection(const int fd_,const struct sockaddr&sa_,const socklen_t salen_,ProcessInterface*process_,void*ctx_,function<void(Connection*)>&&onDestroy_):fd(fd_),salen(salen_),process(process_),ctx(ctx_),onDestroy(forward<function<void(Connection*)>>(onDestroy_)){
  memcpy(&sa,&sa_,sizeof(sa_));
}
TcpConnection::TcpConnection(const int fd,const struct sockaddr&sa,const socklen_t salen,ProcessInterface*process,void*ctx,std::function<void(Connection*)>&&onDestroy):Connection(fd,sa,salen,process,ctx,forward<function<void(Connection*)>>(onDestroy)){}
ssize_t Connection::read(void*buf,size_t count){
  return ::read(fd,buf,count);
}
ssize_t Connection::recv(void *buf,size_t len,int flags){
  return ::recv(fd,buf,len,flags);
}
ssize_t Connection::write(const void*buf,size_t count){
  return ::write(fd,buf,count);
}
void*Connection::handleIn(void*arg){
  return ((Connection*)arg)->process->handleIn(arg);
}
void Connection::handleInCb(void*arg){
  return ((Connection*)arg)->process->handleInCb(arg);
}
void*Connection::handleOut(void*arg){
  return ((Connection*)arg)->process->handleOut(arg);  
}
void Connection::handleOutCb(void*arg){
  return ((Connection*)arg)->process->handleOutCb(arg);
}
Connection::~Connection(){
  onDestroy(this);
}
TcpConnection::~TcpConnection(){
  if(fd>=0)
    close(fd);
}
   
