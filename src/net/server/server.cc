#include<INIReader.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<signal.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<cstring>
#include<string.h>
#include<list>
#include<string>
#include<unordered_set>
#include<pthread.h>
#include"global.h"
#include"thread_pool_rude.h"
#include"thread_pool.h"
#include"connection.h"
#include"epoll.h"
#include"process.h"
#include"rsa.h"
#include"load_tcp_process.h"
#include"spinlock.hpp"
using namespace std;
using namespace elevencent;
enum TCP_BITMAP_TAG:uint8_t{
  TCP_BITMAP_TAG_IN_BUSY     =0b00000001,  
  TCP_BITMAP_TAG_IN_CLOSE    =0b00000100,
  TCP_BITMAP_TAG_OUT_BUSY    =0b00001000,
  TCP_BITMAP_TAG_OUT_CLOSE   =0b00100000,
  TCP_BITMAP_TAG_DELETED     =0b01000000,
  TCP_BITMAP_TAG_EP_MUTEX    =0b10000000,
};
class TcpLoopArg{
public:
  Epoll*ep;
  uint8_t*bitmapFd;
  SpinLock*bitmapFdLock;
  TcpLoopArg(Epoll*ep,uint8_t*bitmapFd,SpinLock*bitmapFdLock):ep(ep),bitmapFd(bitmapFd),bitmapFdLock(bitmapFdLock){}
};
static inline void tcpEpHandleInCb(SpinLock&epOutSetLock,SpinLock&epInSetLock,unordered_set<TcpConnection*>&epInSet,unordered_set<TcpConnection*>&epOutSet,SpinLock*bitmapFdLock,uint8_t*bitmapFd,SpinLock&deletedListLock,list<TcpConnection*>&deletedList,void*arg){
  TcpConnection::handleInCb(arg);
  TcpConnection*conn=(TcpConnection*)arg;
  TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
  int fd=conn->fd;
  while(1){
    bitmapFdLock->lock();
    if(!(bitmapFd[fd]&TCP_BITMAP_TAG_EP_MUTEX)){
      bitmapFd[fd]|=TCP_BITMAP_TAG_EP_MUTEX;
      bitmapFdLock->unlock();
      break;
    }
    bitmapFdLock->unlock();
    sched_yield();
  }
  if(ctx->retIn&TcpProcessContext::RETCODE::CLOSE)
    bitmapFd[fd]|=TCP_BITMAP_TAG_IN_CLOSE|TCP_BITMAP_TAG_OUT_CLOSE;		  
  if(ctx->retIn&TcpProcessContext::RETCODE::SHUT_RD)
    bitmapFd[fd]|=TCP_BITMAP_TAG_IN_CLOSE;
  if(ctx->retIn&TcpProcessContext::RETCODE::SHUT_WR)
    bitmapFd[fd]|=TCP_BITMAP_TAG_OUT_CLOSE;
  if(ctx->retIn&TcpProcessContext::RETCODE::EP_IN){
    epInSetLock.lock();
    epInSet.insert(conn);
    epInSetLock.unlock();	
  }
  if(ctx->retIn&TcpProcessContext::RETCODE::EP_OUT){
    epOutSetLock.lock();
    epOutSet.insert(conn);
    epOutSetLock.unlock();	
  }    
  if(bitmapFd[fd]&TCP_BITMAP_TAG_IN_CLOSE){
    shutdown(fd,SHUT_RD);
    epInSetLock.lock();
    epInSet.erase(conn);
    epInSetLock.unlock();	
    if((bitmapFd[fd]&TCP_BITMAP_TAG_OUT_CLOSE)&&!(bitmapFd[fd]&TCP_BITMAP_TAG_OUT_BUSY)){
      shutdown(fd,SHUT_WR);		  
      bitmapFd[fd]|=TCP_BITMAP_TAG_DELETED;
      deletedListLock.lock();
      deletedList.push_back(conn);
      deletedListLock.unlock();
    }else
      DEBUG_MSG("fd("<<fd<<") IN_CLOSE");
  }
  if(bitmapFd[fd]&TCP_BITMAP_TAG_OUT_CLOSE){
    epOutSetLock.lock();
    epOutSet.erase(conn);
    epOutSetLock.unlock();	
  }
  bitmapFd[fd]&=(~(TCP_BITMAP_TAG_EP_MUTEX|TCP_BITMAP_TAG_IN_BUSY));
}
static inline void tcpEpHandleOutCb(SpinLock&epOutSetLock,SpinLock&epInSetLock,unordered_set<TcpConnection*>&epInSet,unordered_set<TcpConnection*>&epOutSet,SpinLock*bitmapFdLock,uint8_t*bitmapFd,SpinLock&deletedListLock,list<TcpConnection*>&deletedList,void*arg){
  TcpConnection::handleOutCb(arg);
  TcpConnection*conn=(TcpConnection*)arg;
  TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
  int fd=conn->fd;
  while(1){
    bitmapFdLock->lock();
    if(!(bitmapFd[fd]&TCP_BITMAP_TAG_EP_MUTEX)){
      bitmapFd[fd]|=TCP_BITMAP_TAG_EP_MUTEX;
      bitmapFdLock->unlock();
      break;
    }
    bitmapFdLock->unlock();
    sched_yield();    
  }
  if(ctx->retOut&TcpProcessContext::RETCODE::CLOSE)
    bitmapFd[fd]|=TCP_BITMAP_TAG_IN_CLOSE|TCP_BITMAP_TAG_OUT_CLOSE;		  
  if(ctx->retOut&TcpProcessContext::RETCODE::SHUT_RD)
    bitmapFd[fd]|=TCP_BITMAP_TAG_IN_CLOSE;
  if(ctx->retOut&TcpProcessContext::RETCODE::SHUT_WR)
    bitmapFd[fd]|=TCP_BITMAP_TAG_OUT_CLOSE;
  if(ctx->retIn&TcpProcessContext::RETCODE::EP_IN){
    epInSetLock.lock();
    epInSet.insert(conn);
    epInSetLock.unlock();	
  }
  if(ctx->retIn&TcpProcessContext::RETCODE::EP_OUT){
    epOutSetLock.lock();
    epOutSet.insert(conn);
    epOutSetLock.unlock();	
  }    
  if(bitmapFd[fd]&TCP_BITMAP_TAG_OUT_CLOSE){
    shutdown(fd,SHUT_WR);
    epOutSetLock.lock();
    epOutSet.insert(conn);
    epOutSetLock.unlock();	
    if((bitmapFd[fd]&TCP_BITMAP_TAG_IN_CLOSE)&&!(bitmapFd[fd]&TCP_BITMAP_TAG_IN_BUSY)){
      shutdown(fd,SHUT_RD);
      bitmapFd[fd]|=TCP_BITMAP_TAG_DELETED;
      deletedListLock.lock();
      deletedList.push_back(conn);
      deletedListLock.unlock();
    }else
      DEBUG_MSG("fd("<<fd<<") OUT_CLOSE");
  }
  if(bitmapFd[fd]&TCP_BITMAP_TAG_IN_CLOSE){
    epInSetLock.lock();
    epInSet.erase(conn);
    epInSetLock.unlock();	
  }
  bitmapFd[fd]&=(~(TCP_BITMAP_TAG_EP_MUTEX|TCP_BITMAP_TAG_OUT_BUSY));  
}
void*tcpEpollLoop(void*arg){
  TcpLoopArg*loopArg=(TcpLoopArg*)arg;
  Epoll*ep=loopArg->ep;
  uint8_t*bitmapFd=loopArg->bitmapFd;
  SpinLock*bitmapFdLock=loopArg->bitmapFdLock;
  int nfds;
#define MAXEVENTS 512
  struct epoll_event ev[MAXEVENTS];
  #if 0
  ThreadPool thrPool([](ThreadPool*pool,int*thrDatas){
    thrDatas[ThreadPool::THR_DATA_IDX_CACHED]=4;
    thrDatas[ThreadPool::THR_DATA_IDX_MAX]=1000;
  });
  #endif
  ThreadPoolRude thrPool([](ThreadPoolRude*pool,int*thrDatas){
    thrDatas[ThreadPoolRude::ThrDataIdxCached]=4;
    thrDatas[ThreadPoolRude::ThrDataIdxMax]=18;
  });
  uint8_t bitmapEpIn[MAXEVENTS/8+!(MAXEVENTS%8)?0:1];
  uint8_t bitmapEpOut[MAXEVENTS/8+!(MAXEVENTS%8)?0:1];
  SpinLock deletedListLock;
  list<TcpConnection*>deletedList;
  unordered_set<TcpConnection*>epOutSet;
  unordered_set<TcpConnection*>epInSet;
  SpinLock epOutSetLock;
  SpinLock epInSetLock;
  while(1){
    if(unlikely((nfds=ep->wait(ev,MAXEVENTS,0))==-1)){
      //      DEBUG_MSG("ep->wait == -1");
      continue;	
    }
    int epFinishedCount=0;
    for(int i=0;i<nfds;++i){
      BITMAP8_SET(bitmapEpIn,i);
      BITMAP8_SET(bitmapEpOut,i);
      TcpConnection*conn=(TcpConnection*)ev[i].data.ptr;
      int fd=conn->fd;
      bitmapFdLock->lock();
      if(bitmapFd[fd]&TCP_BITMAP_TAG_EP_MUTEX){
	bitmapFdLock->unlock();
	continue;
      }
      bitmapFd[fd]|=TCP_BITMAP_TAG_EP_MUTEX;
      bitmapFdLock->unlock();
      if(bitmapFd[fd]&TCP_BITMAP_TAG_DELETED){
	BITMAP8_UNSET(bitmapEpIn,i);
	BITMAP8_UNSET(bitmapEpOut,i);	
	++epFinishedCount;
	continue;
      }	
      if(ev[i].events&EPOLLHUP)
	bitmapFd[fd]|=TCP_BITMAP_TAG_IN_CLOSE|TCP_BITMAP_TAG_OUT_CLOSE;
      else if(ev[i].events&EPOLLRDHUP)
	bitmapFd[fd]|=TCP_BITMAP_TAG_IN_CLOSE;	
      if(ev[i].events&EPOLLIN){
	epInSetLock.lock();
	epInSet.erase(conn);
	epInSetLock.unlock();		
	if(!(bitmapFd[fd]&TCP_BITMAP_TAG_IN_BUSY)){	  
	  BITMAP8_UNSET(bitmapEpIn,i);
	  bitmapFd[fd]|=TCP_BITMAP_TAG_IN_BUSY;
	  thrPool.pushTask(TcpConnection::handleIn,conn,[&bitmapFdLock,&bitmapFd,&deletedListLock,&deletedList,&epOutSet,&epOutSetLock,&epInSet,&epInSetLock](void*arg){
	    tcpEpHandleInCb(epOutSetLock,epInSetLock,epInSet,epOutSet,bitmapFdLock,bitmapFd,deletedListLock,deletedList,arg);
	  });
	}
      }else
	BITMAP8_UNSET(bitmapEpIn,i);
      if(ev[i].events&EPOLLOUT){
	epOutSetLock.lock();
	epOutSet.erase(conn);
	epOutSetLock.unlock();		
	if(!(bitmapFd[fd]&TCP_BITMAP_TAG_OUT_BUSY)){
	  BITMAP8_UNSET(bitmapEpOut,i);
	  bitmapFd[fd]|=TCP_BITMAP_TAG_OUT_BUSY;
	  thrPool.pushTask(TcpConnection::handleOut,conn,[&bitmapFdLock,&bitmapFd,&deletedListLock,&deletedList,&epOutSetLock,&epInSetLock,&epInSet,&epOutSet](void*arg){
	    tcpEpHandleOutCb(epOutSetLock,epInSetLock,epInSet,epOutSet,bitmapFdLock,bitmapFd,deletedListLock,deletedList,arg);
	  });
	}
      }else
	BITMAP8_UNSET(bitmapEpOut,i);
      if(!BITMAP8_ISSET(bitmapEpIn,i)&&!BITMAP8_ISSET(bitmapEpOut,i))
	++epFinishedCount;
      bitmapFd[fd]&=(~TCP_BITMAP_TAG_EP_MUTEX);
    }
    while(epFinishedCount<nfds){
      for(int i=0;i<nfds;++i){
	if(!BITMAP8_ISSET(bitmapEpIn,i)&&!BITMAP8_ISSET(bitmapEpOut,i))
	  continue;
	TcpConnection*conn=(TcpConnection*)ev[i].data.ptr;
	int fd=conn->fd;
	//	DEBUG_MSG("fd: "<<fd);
	bitmapFdLock->lock();
	if(bitmapFd[fd]&TCP_BITMAP_TAG_EP_MUTEX){
	  bitmapFdLock->unlock();
	  continue;
	}
	bitmapFd[fd]|=TCP_BITMAP_TAG_EP_MUTEX;
	bitmapFdLock->unlock();
	if(bitmapFd[fd]&TCP_BITMAP_TAG_DELETED){
	  BITMAP8_UNSET(bitmapEpIn,i);
	  BITMAP8_UNSET(bitmapEpOut,i);	
	  ++epFinishedCount;
	  continue;
	}		
	if(ev[i].events&EPOLLHUP)
	  bitmapFd[fd]|=TCP_BITMAP_TAG_IN_CLOSE|TCP_BITMAP_TAG_OUT_CLOSE;
	else if(ev[i].events&EPOLLRDHUP)
	  bitmapFd[fd]|=TCP_BITMAP_TAG_IN_CLOSE;	
	if((ev[i].events&EPOLLIN)&&BITMAP8_ISSET(bitmapEpIn,i)){
	  epInSetLock.lock();
	  epInSet.erase(conn);
	  epInSetLock.unlock();		  
	  if(!(bitmapFd[fd]&TCP_BITMAP_TAG_IN_BUSY)){
	    BITMAP8_UNSET(bitmapEpIn,i);
	    bitmapFd[fd]|=TCP_BITMAP_TAG_IN_BUSY;
	    thrPool.pushTask(TcpConnection::handleIn,conn,[&bitmapFdLock,&bitmapFd,&deletedListLock,&deletedList,&epOutSetLock,&epInSetLock,&epInSet,&epOutSet](void*arg){
	      tcpEpHandleInCb(epOutSetLock,epInSetLock,epInSet,epOutSet,bitmapFdLock,bitmapFd,deletedListLock,deletedList,arg);		
	    });
	  }
	}else
	  BITMAP8_UNSET(bitmapEpIn,i);
	if((ev[i].events&EPOLLOUT)&&BITMAP8_ISSET(bitmapEpOut,i)){
	  epOutSetLock.lock();
	  epOutSet.erase(conn);
	  epOutSetLock.unlock();		  
	  if(!(bitmapFd[fd]&TCP_BITMAP_TAG_OUT_BUSY)){
	    BITMAP8_UNSET(bitmapEpOut,i);
	    bitmapFd[fd]|=TCP_BITMAP_TAG_OUT_BUSY;
	    thrPool.pushTask(TcpConnection::handleOut,conn,[&bitmapFdLock,&bitmapFd,&deletedListLock,&deletedList,&epOutSetLock,&epInSetLock,&epInSet,&epOutSet](void*arg){
	      tcpEpHandleOutCb(epOutSetLock,epInSetLock,epInSet,epOutSet,bitmapFdLock,bitmapFd,deletedListLock,deletedList,arg);
	    });
	  }
	}else
	  BITMAP8_UNSET(bitmapEpOut,i);
	if(!BITMAP8_ISSET(bitmapEpIn,i)&&!BITMAP8_ISSET(bitmapEpOut,i))
	  ++epFinishedCount;
	bitmapFd[fd]&=(~TCP_BITMAP_TAG_EP_MUTEX);	
      }
    }
    if(deletedList.empty())
      continue;
    deletedListLock.lock();
    for(auto&item:deletedList){
      DEBUG_MSG("fd: "<<item->fd<<"\nCLOSED");
      bitmapFd[item->fd]=0;
      delete item;      
    }
    deletedList.clear();
    deletedListLock.unlock();    
    continue;
    epInSetLock.lock();
    
    for(auto&item:epInSet){
      thrPool.pushTask(TcpConnection::handleIn,item,[&bitmapFdLock,&bitmapFd,&deletedListLock,&deletedList,&epOutSet,&epOutSetLock,&epInSet,&epInSetLock](void*arg){
	tcpEpHandleInCb(epOutSetLock,epInSetLock,epInSet,epOutSet,bitmapFdLock,bitmapFd,deletedListLock,deletedList,arg);
      });
    }
    epInSetLock.unlock();
    epOutSetLock.lock();
    for(auto&item:epOutSet){
      
      thrPool.pushTask(TcpConnection::handleOut,item,[&bitmapFdLock,&bitmapFd,&deletedListLock,&deletedList,&epOutSet,&epOutSetLock,&epInSet,&epInSetLock](void*arg){
	tcpEpHandleOutCb(epOutSetLock,epInSetLock,epInSet,epOutSet,bitmapFdLock,bitmapFd,deletedListLock,deletedList,arg);
      });
    }
    epOutSetLock.unlock();    
  }
  return 0;
}
void tcpLoop(Socket&sock){
  static Epoll ep;
  if(ep.epfd<0)exit(-1);
#define BITMAP_FD_LEN 1048576
  uint8_t bitmapFd[BITMAP_FD_LEN]={0};
  SpinLock bitmapFdLock;
  TcpLoopArg loopArg(&ep,bitmapFd,&bitmapFdLock);
  {
    pthread_t pid;
    if(pthread_create(&pid,0,tcpEpollLoop,&loopArg)){
      DEBUG_MSG("error");
      exit(1);
    }
  }
  ProcessInterface*process=new TcpProcess;
  if(fcntl(sock.fd,F_SETFL,fcntl(sock.fd,F_GETFL)&(~O_NONBLOCK))==-1){
    DEBUG_MSG("fcntl==-1, fd: "<<sock.fd);
    exit(1);
  };
  while(1){
    struct sockaddr sa;
    socklen_t salen;
    int fd=sock.accept(&sa,&salen);
    //    DEBUG_MSG("accept fd: "<<fd);
    if(likely(fd>=0)){
      if(unlikely(fd>=BITMAP_FD_LEN)){
	DEBUG_MSG("fd("<<fd<<") >= BITMAP_FD_LEN("<<BITMAP_FD_LEN<<")");
	close(fd);
	continue;
      }
      if(unlikely(fcntl(fd,F_SETFL,fcntl(fd,F_GETFL)|O_NONBLOCK)==-1)){
	DEBUG_MSG("fcntl==-1, fd: "<<fd);
	close(fd);
	continue;
      }
      struct epoll_event ev;
      ev.events=EPOLLIN|EPOLLET|EPOLLRDHUP|EPOLLOUT;
      TcpConnection*conn=new TcpConnection(fd,sa,salen,process,new TcpProcessContext(ev.events,&ep),[](void*arg){
	TcpConnection*conn=(TcpConnection*)arg;
	delete ((TcpProcessContext*)conn->ctx);
      });
      ev.data.ptr=conn;
      if(unlikely(ep.ctl(EPOLL_CTL_ADD,fd,&ev)==-1)){
	DEBUG_MSG("ep.ctl==-1, fd: "<<fd);
	delete conn;
      }
    }
  }
}
void udpLoop(Socket&sock){}
int main(int argc,char**argv){
  if(argc<2)
    cerr<<"usage: <"<<argv[0]<<"> <ini-path>"<<endl,exit(-1);
  string l4type;
  uint16_t l4port;
  {
    INIReader ini(argv[1]);
    string section="server";
    string name="l4type";
    l4type=ini.GetString(section,name,"tcp");
    if(l4type!="tcp"&&l4type!="udp"){
      DEBUG_MSG("error: unknown l4type("<<l4type<<")"<<endl);
      return -1;
    }
    name="l4port";
    l4port=ini.GetInteger(section,name,10100);
  }  
  Socket sock(l4type=="tcp"?SOCK_STREAM:SOCK_DGRAM,l4port);
  if(sock.fd<0)return -1;
  signal(SIGPIPE,SIG_IGN);  
  l4type=="tcp"?tcpLoop(sock):udpLoop(sock);
}
