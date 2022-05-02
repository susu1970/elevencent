#include"connection.h"
#include<string.h>
#include<sys/socket.h>
#include<netdb.h>
using namespace elevencent;

Connection::Connection(const int fd_,const struct sockaddr&sa_,const socklen_t salen_):fd(fd_),salen(salen_){
  memcpy(&sa,&sa_,sizeof(sa_));
}
TcpConnection::TcpConnection(const int fd_,const struct sockaddr&sa_,const socklen_t salen_):Connection(fd_,sa_,salen_){
  
}
int Connection::getnameinfo(char*host,socklen_t hostlen,char*serv,socklen_t servlen,int flags){
  return ::getnameinfo(&sa,salen,host,hostlen,serv,servlen,flags);
}
void*Connection::handle(void*arg){

}
void Connection::handleCb(void*arg){
  
}
void*TcpConnection::handle(void*arg){
  if(unlikely(!arg)){
    DEBUG_MSG("error");
    return 0; 
  }
  TcpConnection*conn=(TcpConnection*)arg;
  char host[128]={0};
  char serv[128]={0};
  conn->getnameinfo(host,128,serv,128,0);
  DEBUG_MSG("fd: "<<conn->fd<<"\nhost: "<<host<<"\nserv: "<<serv);
  return conn;
}
void TcpConnection::handleCb(void*arg){
  if(unlikely(!arg)){
    DEBUG_MSG("error");
    return;    
  }
  TcpConnection*conn=(TcpConnection*)arg;  
  DEBUG_MSG("fd: "<<conn->fd<<" finished!");
}
static void handleCb(void*arg);    
TcpConnection::~TcpConnection(){
  if(fd>=0)close(fd);
}
