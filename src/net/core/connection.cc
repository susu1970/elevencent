#include"connection.h"
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netdb.h>
#include<unistd.h>
using namespace elevencent;
Connection::Buf::Buf(size_t maxlen_),maxlen(maxlen_),buf(0),len(0){}
Connection::Buf::~Buf(){if(buf)free(buf);}
bool Connection::Buf::append(const void*src,size_t n){
  if(unlikely(!n))return true;
  if(!len){
    if(unlikely(n>maxlen))return false;
    buf=malloc(n);
    if(unlikely(!buf))return false;
    memcpy(buf,src,len=n);
    return true;
  }
  ssize_t avail=maxlen-len;
  if(avail<n)return false;
  char*newBuf=malloc(len+n);
  if(unlikely(!newBuf))return false;
  memcpy(newBuf,buf,len);
  memcpy(newBuf+len,src,n);
  len+=n;
  free(buf);
  buf=newBuf;
  return true;
}
size_t Connection::Buf::popFront(void*dst,size_t n){
  if(!buf)return 0;
  if(n>=len){
    memcpy(dst,buf,len);
    delete buf;
    buf=0;
    n=len;
    len=0;
    return n;
  }
  char*newBuf=malloc(len-n);
  if(unlikely(!newBuf))return 0;
  memcpy(dst,buf,n);
  memcpy(newBuf,buf+n,len-n);
  delete buf;
  buf=newBuf;
  len-=n;
  return n;
}
void*Connection::Buf::peekBuf()const{return buf;}
size_t Connection::Buf::size(){return len;}
Connection::Connection(const int fd_,const struct sockaddr&sa_,const socklen_t salen_):fd(fd_),salen(salen_){memcpy(&sa,&sa_,sizeof(sa_));}
TcpConnection::TcpConnection(const int fd_,const struct sockaddr&sa_,const socklen_t salen_,Epoll*ep_):Connection(fd_,sa_,salen_),ep(ep_),inbuf(0),inlen(0),inoff(0),outbuf(0),outlen(0),outoff(0){}
int Connection::getnameinfo(char*host,socklen_t hostlen,char*serv,socklen_t servlen,int flags){return ::getnameinfo(&sa,salen,host,hostlen,serv,servlen,flags);}
ssize_t TcpConnection::write(const void*buf,size_t count){return ::write(fd,buf,count);}
ssize_t TcpConnection::read(void*buf,size_t count){return ::read(fd,buf,count);}
void*TcpConnection::handleIn(void*arg){
  TcpConnection*conn=(TcpConnection*)arg;
  ssize_t sz;
  while((sz=conn->read())>0);
  
  if(conn->read())
  char buf[4096];
  buf[0]=0;
  while(1){
    
  }
  return arg;
}
void TcpConnection::handleInCb(void*arg){
  TcpConnection*conn=(TcpConnection*)arg;
}
void*TcpConnection::handleOut(void*arg){
  TcpConnection*conn=(TcpConnection*)arg;
  return arg;
}
void TcpConnection::handleOutCb(void*arg){
  TcpConnection*conn=(TcpConnection*)arg;
}
TcpConnection::~TcpConnection(){
  if(fd>=0)close(fd);
  if(inlen)delete inbuf;
  if(outlen)delete outbuf;  
}
