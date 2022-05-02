#include"global.h"
#include"thread_pool.h"
#include"connection.h"
#include"epoll.h"

#include<unistd.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<cstring>
#include<string.h>
#include<string>

using namespace std;
using namespace elevencent;

void*tcpEpollLoop(void*arg){
  Epoll*ep=(Epoll*)arg;  
  int nfds;
#define MAXEVENTS 512
  struct epoll_event ev[MAXEVENTS];
  ThreadPool thrPool;
  thrPool.setThrDataFunc([g_processNum](ThreadPool*tp,short*A){
    A[ThrDataIdxCached]=1;
    A[ThrDataIdxMax]=g_processNum+1;      
  });  
  while(1){
    if(unlikely((nfds=ep->wait(ev,MAXEVENTS,0))==-1)){
      DEBUG_MSG("ep->wait == -1");
      continue;	
    }
    for(int i=0;i<nfds;++i)
      thrPool.pushTask(TcpConnection::handle,ev[i].data.ptr,TcpConnection::handleCb,TaskNice::TaskNiceDft,false);
  }
  return 0;
};
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
  while(1){
    struct sockaddr sa;
    socklen_t salen;    
    int fd=sock.accept(&sa,&salen);
    if(likely(fd>=0)){
      DEBUG_MSG("accepted fd: "<<fd);
      TcpConnection*conn=new TcpConnection(fd,sa,salen);
      if(unlikely(!conn)){
	close(fd);
	continue;
      }
      if(unlikely(fcntl(fd,F_SETFD,fcntl(fd,F_GETFD)|O_NONBLOCK)==-1)){
	close(fd);
	continue;
      }
      struct epoll_event ev;
      ev.data.ptr=conn;
      ev.events=EPOLLIN|EPOLLET|EPOLLOUT;
      if(unlikely(ep.ctl(EPOLL_CTL_ADD,fd,&ev)==-1)){
	delete conn;
	close(fd);
      }
    }
  }
}
void udpLoop(const Socket&sock){}
int main(int argc,char**argv){
  if(argc<2||(strcmp(argv[1],"tcp")&&strcmp(argv[1],"udp")))
    cerr<<"usage: <"<<argv[0]<<"> <tcp>/<udp> /<port,default 10100>"<<endl,exit(-1);
  uint16_t port=10100;
  if(argc>2)port=atoi(argv[2]);
  Socket sock(((!strcmp(argv[1],"tcp"))?SOCK_STREAM:SOCK_DGRAM),port);
  if(sock.fd<0)return -1;
  !strcmp(argv[1],"tcp")?tcpLoop(sock):udpLoop(sock);
}
