#include<INIReader.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<signal.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<cstring>
#include<string.h>
#include<string>
#include"global.h"
#include"thread_pool.h"
#include"connection.h"
#include"epoll.h"
#include"process.h"
#include"rsa.h"
#include"load_tcp_process.h"
using namespace std;
using namespace elevencent;
  
void*tcpEpollLoop(void*arg){
  Epoll*ep=(Epoll*)arg;  
  int nfds;
#define MAXEVENTS 512
  struct epoll_event ev[MAXEVENTS];
  ThreadPool thrPool;
  thrPool.setThrDataFunc([](ThreadPool*tp,short*A){
    A[ThrDataIdxCached]=1;
    A[ThrDataIdxMax]=g_processNum+1;      
  });
  while(1){
    if(unlikely((nfds=ep->wait(ev,MAXEVENTS,0))==-1)){
      DEBUG_MSG("ep->wait == -1");
      continue;	
    }
    for(int i=0;i<nfds;++i){
      if(ev[i].events&EPOLLOUT)
	thrPool.pushTask(TcpConnection::handleOut,ev[i].data.ptr,TcpConnection::handleOutCb,TaskNice::TaskNiceDft,false);	
      if(ev[i].events&EPOLLIN)
	thrPool.pushTask(TcpConnection::handleIn,ev[i].data.ptr,TcpConnection::handleInCb,TaskNice::TaskNiceDft,false);
    }
  }
  return 0;
}
void tcpLoop(Socket&sock){
  static Epoll ep;
  if(ep.epfd<0)exit(-1);
  {
    pthread_t pid;
    if(pthread_create(&pid,0,tcpEpollLoop,&ep)){
      DEBUG_MSG("error");
      exit(1);
    }
  }
  ProcessInterface*process=new TcpProcess;
  while(1){
    struct sockaddr sa;
    socklen_t salen;    
    int fd=sock.accept(&sa,&salen);
    if(likely(fd>=0)){
      TcpConnection*conn=new TcpConnection(fd,sa,salen,process,new TcpProcessContext(&ep),[](void*arg){
	TcpConnection*conn=(TcpConnection*)arg;
	delete ((TcpProcessContext*)conn->ctx);
      });
      if(unlikely(fcntl(fd,F_SETFD,fcntl(fd,F_GETFD)|O_NONBLOCK)==-1)){
	delete conn;
	continue;
      }
      struct epoll_event ev;
      ev.data.ptr=conn;
      ev.events=EPOLLIN|EPOLLET;
      if(unlikely(ep.ctl(EPOLL_CTL_ADD,fd,&ev)==-1))
	delete conn;
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
  signal(SIGPIPE, SIG_IGN);  
  l4type=="tcp"?tcpLoop(sock):udpLoop(sock);
}
