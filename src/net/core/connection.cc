#include"connection.h"
#include"global.h"
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
ssize_t Connection::read(void*buf,size_t count,int*err){
  if(!err)
    return ::read(fd,buf,count);
  *err=ERRNO_OK;
  ssize_t n;
  uint64_t left=count,off=0;
  uint64_t limit=inQos?inQos->peekLimit():left;
  if(left>limit){
    *err|=ERRNO_RLIMIT_QOS;
    left=limit;
  }
  while(left>0){
    while(left>0&&(n=::read(fd,buf+off,left))>0){
      left-=n;
      off+=n;
    }
    if(n==-1){
      if(errno==EINTR)
	continue;
      if(errno==EAGAIN){
	*err|=ERRNO_AGAIN;
	break;
      }
      *err|=ERRNO_CLOSE;
      break;
    }
    if(!n){
      *err|=ERRNO_CLOSE;
      break;
    }
  }
  if(inQos)
    inQos->consumeLimit(off);
  return off;
}
ssize_t Connection::recv(void *buf,size_t len,int flags){
  return ::recv(fd,buf,len,flags);
}
ssize_t Connection::write(const void*buf,size_t count,int*err){
  if(!err)
    return ::write(fd,buf,count);
  ssize_t n;
  uint64_t left=count,off=0;
  *err=ERRNO_OK;
  uint64_t limit=outQos?outQos->peekLimit():left;
  if(left>limit){
    *err|=ERRNO_RLIMIT_QOS;
    left=limit;
  }
  while(left>0){
    while(left>0&&(n=::write(fd,buf+off,left))>0){
      left-=n;
      off+=n;
    }
    if(errno==EPIPE){
      *err|=ERRNO_CLOSE;
      break;
    }
    if(errno==EINTR)
      continue;
    if(!n||errno==EAGAIN){
      *err|=ERRNO_AGAIN;
      break;
    }
  }
  if(outQos)
    outQos->consumeLimit(off);
  return off;
}
void*Connection::handleIn(void*arg){
  Connection*conn=(Connection*)arg;
  if(conn->inCpu)
    conn->inCpu->updateCurUsec();
  return conn->process->handleIn(arg);
}
void Connection::handleInCb(void*arg){
  return ((Connection*)arg)->process->handleInCb(arg);
}
void*Connection::handleOut(void*arg){
  Connection*conn=(Connection*)arg;
  if(conn->outCpu)
    conn->outCpu->updateCurUsec();
  return conn->process->handleOut(arg);  
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
