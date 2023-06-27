#include<unistd.h>
#include<sys/resource.h>
#include<fcntl.h>
#include<sched.h>
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
#include"resource.h"
#include"rsa.h"
#include"load_tcp_process.h"
#include"spinlock.hpp"
#include"ini_parser.hpp"
#include"resource_global.h"
#include"resource_limit.hpp"
using namespace std;
using namespace elevencent;
using namespace elevencent::ResourceLimit;
string elevencent::g_privkey="";
string elevencent::g_pubkey="";
RSA::Key elevencent::g_keypub;
RSA::Key elevencent::g_keypriv;
int64_t elevencent::ResourceLimit::g_cpuUsecs[ResourceLimit::Cpu::LIMIT_KEY::MAX]={-1};
Frequency::Rule elevencent::ResourceLimit::g_rules[Frequency::DIMENSION_MAX];
Size elevencent::ResourceLimit::g_sizeLimit;
Resource*elevencent::g_resource;

static uint64_t s_qos_in_min=1600,s_qos_in_rate=-1ul;
static uint64_t s_qos_out_min=1600,s_qos_out_rate=-1ul;
typedef uint16_t tcp_bitmap_tag_t;
enum TCP_BITMAP_TAG:tcp_bitmap_tag_t{
  TCP_BITMAP_TAG_IN_AGAIN=1,  
  TCP_BITMAP_TAG_IN_CLOSE=1<<2,
  TCP_BITMAP_TAG_IN_DEAL_ONCE=1<<3,  
  TCP_BITMAP_TAG_IN_FINISHED=1<<4,
  TCP_BITMAP_TAG_IN_EVENT_INSERT=1<<10,
  TCP_BITMAP_TAG_IN_BUSY=1<<11,    

  TCP_BITMAP_TAG_OUT_AGAIN=1<<5,  
  TCP_BITMAP_TAG_OUT_DEAL_ONCE=1<<6,  
  TCP_BITMAP_TAG_OUT_CLOSE=1<<7,
  TCP_BITMAP_TAG_OUT_FINISHED=1<<8,
  TCP_BITMAP_TAG_OUT_EVENT_INSERT=1<<9,
  TCP_BITMAP_TAG_OUT_BUSY=1<<12,      
};

class TcpLoopArg{
public:
  Epoll*ep;
  volatile tcp_bitmap_tag_t*bitmapFd;
  volatile int8_t*fdLocks;
};
static inline void tcpEpHandleInCb(TcpLoopArg*loopArg,TcpConnection*conn){
  TcpConnection::handleInCb((void*)conn);
  TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
  int fd=conn->fd;
  volatile int8_t*fdLocks=loopArg->fdLocks;
  volatile tcp_bitmap_tag_t*bitmapFd=loopArg->bitmapFd;  
  while(!CAS_BOOL(&fdLocks[fd],0,1))
    sched_yield();//加锁
  bitmapFd[fd]&=~TCP_BITMAP_TAG_IN_AGAIN;
  bitmapFd[fd]&=~TCP_BITMAP_TAG_IN_BUSY;      
  bitmapFd[fd]|=TCP_BITMAP_TAG_IN_FINISHED;  
  if(ctx->retIn&TcpProcessContext::RETCODE::IN_AGAIN){
    bitmapFd[fd]|=TCP_BITMAP_TAG_IN_AGAIN;
  }
  if(ctx->retIn&TcpProcessContext::RETCODE::OUT_AGAIN){
    bitmapFd[fd]|=TCP_BITMAP_TAG_OUT_AGAIN;
  }
  if(ctx->retIn&TcpProcessContext::RETCODE::IN_INSERT_EV){
    bitmapFd[fd]|=TCP_BITMAP_TAG_IN_EVENT_INSERT;
  }
  if(ctx->retIn&TcpProcessContext::RETCODE::OUT_INSERT_EV){
    bitmapFd[fd]|=TCP_BITMAP_TAG_OUT_EVENT_INSERT;
  }  
  if(ctx->retIn&TcpProcessContext::RETCODE::IN_CLOSE){
    bitmapFd[fd]&=~TCP_BITMAP_TAG_IN_AGAIN;
    bitmapFd[fd]&=~TCP_BITMAP_TAG_IN_EVENT_INSERT;        
    bitmapFd[fd]|=TCP_BITMAP_TAG_IN_CLOSE;    
  }
  if(ctx->retIn&TcpProcessContext::RETCODE::OUT_CLOSE){
    bitmapFd[fd]&=~TCP_BITMAP_TAG_OUT_AGAIN;
    bitmapFd[fd]&=~TCP_BITMAP_TAG_OUT_EVENT_INSERT;            
    bitmapFd[fd]|=TCP_BITMAP_TAG_OUT_CLOSE;    
  }
  fdLocks[fd]=0;//解锁  
}
static inline void tcpEpHandleOutCb(TcpLoopArg*loopArg,TcpConnection*conn){
  TcpConnection::handleOutCb((void*)conn);
  TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
  int fd=conn->fd;
  volatile int8_t*fdLocks=loopArg->fdLocks;
  volatile tcp_bitmap_tag_t*bitmapFd=loopArg->bitmapFd;  
  while(!CAS_BOOL(&fdLocks[fd],0,1))
    sched_yield();//加锁
  bitmapFd[fd]&=~TCP_BITMAP_TAG_OUT_AGAIN;
  bitmapFd[fd]&=~TCP_BITMAP_TAG_OUT_BUSY;        
  bitmapFd[fd]|=TCP_BITMAP_TAG_OUT_FINISHED;
  if(ctx->retOut&TcpProcessContext::RETCODE::IN_AGAIN){
    bitmapFd[fd]|=TCP_BITMAP_TAG_IN_AGAIN;
  }
  if(ctx->retOut&TcpProcessContext::RETCODE::OUT_AGAIN){
    bitmapFd[fd]|=TCP_BITMAP_TAG_OUT_AGAIN;
  }
  if(ctx->retOut&TcpProcessContext::RETCODE::IN_INSERT_EV){
    bitmapFd[fd]|=TCP_BITMAP_TAG_IN_EVENT_INSERT;
  }
  if(ctx->retOut&TcpProcessContext::RETCODE::OUT_INSERT_EV){
    bitmapFd[fd]|=TCP_BITMAP_TAG_OUT_EVENT_INSERT;
  }  
  if(ctx->retOut&TcpProcessContext::RETCODE::IN_CLOSE){
    bitmapFd[fd]&=~TCP_BITMAP_TAG_IN_AGAIN;
    bitmapFd[fd]&=~TCP_BITMAP_TAG_IN_EVENT_INSERT;        
    bitmapFd[fd]|=TCP_BITMAP_TAG_IN_CLOSE;    
  }
  if(ctx->retOut&TcpProcessContext::RETCODE::OUT_CLOSE){
    bitmapFd[fd]&=~TCP_BITMAP_TAG_OUT_AGAIN;
    bitmapFd[fd]&=~TCP_BITMAP_TAG_OUT_EVENT_INSERT;            
    bitmapFd[fd]|=TCP_BITMAP_TAG_OUT_CLOSE;    
  }
  fdLocks[fd]=0;//解锁  
}
void*tcpEpollLoop(void*arg){
  TcpLoopArg*loopArg=(TcpLoopArg*)arg;
  Epoll*ep=loopArg->ep;
  volatile tcp_bitmap_tag_t*fdBitmap=loopArg->bitmapFd;
  volatile int8_t*fdLocks=loopArg->fdLocks;
  int nfds;
#define MAXEVENTS 512
  struct epoll_event ev[MAXEVENTS]={0};
  uint8_t epfdFinishedBitmap[MAXEVENTS/8+1]={0};
  ThreadPoolRude thrPool([](ThreadPoolRude*pool,int*thrDatas){
    thrDatas[ThreadPoolRude::ThrDataIdxCached]=4;
    thrDatas[ThreadPoolRude::ThrDataIdxMax]=18;
  });
  unordered_set<uint64_t>inEvs,outEvs;
  while(1){
    if((nfds=ep->wait(ev,MAXEVENTS,0))<1){//construct events by ourself
      unordered_map<uint64_t,int>um;
      nfds=0;
      for(auto iter=inEvs.begin();iter!=inEvs.end()&&nfds<MAXEVENTS;++nfds,++iter){
	ev[nfds].events=EPOLLIN;
	um[*iter]=nfds;
	ev[nfds].data.ptr=(void*)(*iter);
      }
      for(auto iter=outEvs.begin();iter!=outEvs.end()&&nfds<MAXEVENTS;++iter){
	if(um.find(*iter)!=um.end()){
	  ev[um[*iter]].events|=EPOLLOUT;
	}else{
	  ev[nfds].events|=EPOLLOUT;
	  ev[nfds].data.ptr=(void*)(*iter);
	  ++nfds;
	}
      }
      if(nfds<1)
	nfds=ep->wait(ev,MAXEVENTS,-1);//no more events,block ourself for power saving
    }
  tcp_bitmap_tag_again:
    for(int i=0;i<nfds;++i){
      if(BITMAP8_ISSET(epfdFinishedBitmap,i))
	continue;	
      TcpConnection*conn=(TcpConnection*)ev[i].data.ptr;
      int fd=conn->fd;
      if(!CAS_BOOL(&fdLocks[fd],0,1))//当前fd正被处理中
	continue;
      if(fdBitmap[fd]&TCP_BITMAP_TAG_IN_FINISHED&&fdBitmap[fd]&TCP_BITMAP_TAG_OUT_FINISHED){
	BITMAP8_SET(epfdFinishedBitmap,i);
	fdLocks[fd]=0;//解除对fd占用
	continue;
      }
      if(ev[i].events&EPOLLHUP){
	fdBitmap[fd]|=TCP_BITMAP_TAG_OUT_CLOSE|TCP_BITMAP_TAG_IN_CLOSE;
      }else if(ev[i].events&EPOLLRDHUP){
	fdBitmap[fd]|=TCP_BITMAP_TAG_IN_CLOSE;
      }
      bool pushTaskIn=false,pushTaskOut=false;
      do{
	if(fdBitmap[fd]&TCP_BITMAP_TAG_IN_BUSY)
	  break;
	if(fdBitmap[fd]&TCP_BITMAP_TAG_IN_AGAIN){
	  fdBitmap[fd]|=TCP_BITMAP_TAG_IN_DEAL_ONCE;
	  pushTaskIn=true;
	}else{
	  if(ev[i].events&EPOLLIN){
	    if(!(fdBitmap[fd]&TCP_BITMAP_TAG_IN_DEAL_ONCE)){
	      fdBitmap[fd]|=TCP_BITMAP_TAG_IN_DEAL_ONCE;
	      pushTaskIn=true;
	    }
	  }else
	    fdBitmap[fd]|=TCP_BITMAP_TAG_IN_FINISHED;	    	    
	} 	
      }while(0);
      do{
	if(fdBitmap[fd]&TCP_BITMAP_TAG_OUT_BUSY)
	  break;
	if(fdBitmap[fd]&TCP_BITMAP_TAG_OUT_AGAIN){
	  fdBitmap[fd]|=TCP_BITMAP_TAG_OUT_DEAL_ONCE;
	  pushTaskOut=true;
	}else{
	  if(ev[i].events&EPOLLOUT){
	    if(!(fdBitmap[fd]&TCP_BITMAP_TAG_OUT_DEAL_ONCE)){
	      fdBitmap[fd]|=TCP_BITMAP_TAG_OUT_DEAL_ONCE;
	      pushTaskOut=true;
	    }
	  }else
	    fdBitmap[fd]|=TCP_BITMAP_TAG_OUT_FINISHED;
	}	
      }while(0);
      if(pushTaskIn)
	fdBitmap[fd]|=TCP_BITMAP_TAG_IN_BUSY;
      if(pushTaskOut)
	fdBitmap[fd]|=TCP_BITMAP_TAG_OUT_BUSY;
      fdLocks[fd]=0;//解锁      
      if(pushTaskIn){
	thrPool.pushTask(TcpConnection::handleIn,conn,[loopArg](void*arg){
	  tcpEpHandleInCb(loopArg,(TcpConnection*)arg);
	});
      }
      if(pushTaskOut){
	thrPool.pushTask(TcpConnection::handleOut,conn,[loopArg](void*arg){
	  tcpEpHandleOutCb(loopArg,(TcpConnection*)arg);
	});
      }
    }
    for(int i=0;i<nfds;++i){
      if(!BITMAP8_ISSET(epfdFinishedBitmap,i))//有未完成的事件
	goto tcp_bitmap_tag_again;	
    }
    for(int i=0;i<nfds;++i){//某些连接需要再次调用事件处理回调
      TcpConnection*conn=(TcpConnection*)ev[i].data.ptr;
      int fd=conn->fd;
      if(fdBitmap[fd]&TCP_BITMAP_TAG_IN_AGAIN)
	fdBitmap[fd]&=~TCP_BITMAP_TAG_IN_FINISHED;
      if(fdBitmap[fd]&TCP_BITMAP_TAG_OUT_AGAIN)
	fdBitmap[fd]&=~TCP_BITMAP_TAG_OUT_FINISHED;      
      if(fdBitmap[fd]&TCP_BITMAP_TAG_IN_AGAIN||fdBitmap[fd]&TCP_BITMAP_TAG_OUT_AGAIN){//继续处理	
	BITMAP8_UNSET(epfdFinishedBitmap,i);      
	goto tcp_bitmap_tag_again;
      }
    }
    for(int i=0;i<nfds;++i){//所有事件处理完成
      BITMAP8_UNSET(epfdFinishedBitmap,i);            
      TcpConnection*conn=(TcpConnection*)ev[i].data.ptr;
      int fd=conn->fd;
      if(fdBitmap[fd]&TCP_BITMAP_TAG_IN_CLOSE||fdBitmap[fd]&TCP_BITMAP_TAG_OUT_CLOSE){
	ep->ctl(EPOLL_CTL_DEL,fd,0);
	//	DEBUG_MSG("fd: "<<fd<<" closed");
	inEvs.erase((uint64_t)conn);
	outEvs.erase((uint64_t)conn);	
	delete conn;
      }else{
	if(fdBitmap[fd]&TCP_BITMAP_TAG_IN_EVENT_INSERT){
	  inEvs.insert((uint64_t)conn);
	}
	else
	  inEvs.erase((uint64_t)conn);
	if(fdBitmap[fd]&TCP_BITMAP_TAG_OUT_EVENT_INSERT){
	  outEvs.insert((uint64_t)conn);	  
	}
	else
	  outEvs.erase((uint64_t)conn);
      }
      fdBitmap[fd]=0;//清理fd状态
    }
  }
  return 0;
}
void tcpLoop(Socket&sock){
  Epoll ep;
  if(ep.epfd<0)exit(-1);
#define TCP_BITMAP_FD_LEN 104857
  volatile tcp_bitmap_tag_t bitmapFd[TCP_BITMAP_FD_LEN]={0};
  volatile int8_t fdLocks[TCP_BITMAP_FD_LEN]={0};
  TcpLoopArg loopArg;
  loopArg.ep=&ep;
  loopArg.bitmapFd=bitmapFd;
  loopArg.fdLocks=fdLocks;  
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
  }
  struct epoll_event ev;
  ev.events=EPOLLIN|EPOLLET|EPOLLRDHUP|EPOLLOUT;
  while(1){
    struct sockaddr sa;
    socklen_t salen;
    int fd=sock.accept(&sa,&salen);
    if(unlikely(fd<0))
      continue;
    if(unlikely(fd>=TCP_BITMAP_FD_LEN)){
      DEBUG_MSG("fd("<<fd<<") >= TCP_BITMAP_FD_LEN("<<TCP_BITMAP_FD_LEN<<")");
      close(fd);
      continue;
    }
    if(unlikely(fcntl(fd,F_SETFL,fcntl(fd,F_GETFL)|O_NONBLOCK)==-1)){
      DEBUG_MSG("fcntl==-1, fd: "<<fd);
      close(fd);
      continue;
    }
    TcpProcessContext*ctx=new TcpProcessContext();
    ctx->inFreq.setRules(g_rules);
    ctx->outFreq.setRules(g_rules);
    ctx->outList.fuzzyMaxSize=g_sizeLimit["tcp_process_out_queue_size"];
    ResourceLimit::Qos*inQos=new ResourceLimit::Qos(s_qos_in_min,s_qos_in_rate);
    ResourceLimit::Qos*outQos=new ResourceLimit::Qos(s_qos_out_min,s_qos_out_rate);
    ResourceLimit::Cpu*inCpu=new ResourceLimit::Cpu(g_cpuUsecs);
    ResourceLimit::Cpu*outCpu=new ResourceLimit::Cpu(g_cpuUsecs);
    TcpConnection*conn=new TcpConnection(fd,sa,salen,process,ctx,[ctx,inQos,outQos,inCpu,outCpu](void*arg){
      TcpConnection*conn=(TcpConnection*)arg;
      if((void*)conn->ctx==(void*)ctx)
	delete ((TcpProcessContext*)conn->ctx);
      if(conn->inQos==inQos)
	delete inQos;
      if(conn->outQos==outQos)
	delete outQos;
      if(conn->inCpu==inCpu)
	delete inCpu;
      if(conn->outCpu==outCpu)
	delete outCpu;
    });
    conn->inCpu=inCpu;
    conn->outCpu=outCpu;
    conn->inQos=inQos;
    conn->outQos=outQos;
    ev.data.ptr=conn;
    if(unlikely(ep.ctl(EPOLL_CTL_ADD,fd,&ev)==-1)){
      DEBUG_MSG("ep.ctl==-1, fd: "<<fd);
      delete conn;
    }
    //    DEBUG_MSG("fd: "<<fd<<" estabed");    
  }
}
void udpLoop(Socket&sock){}
int main(int argc,char**argv){
  if(argc<2)
    cerr<<"usage: <"<<argv[0]<<"> <ini-path>"<<endl,exit(-1);
  srand(time(nullptr));  
  {
    //ulimit -s unlimited
    struct rlimit sLimit={
      .rlim_cur=RLIM_INFINITY,
      .rlim_max=RLIM_INFINITY,
    };
    if(setrlimit(RLIMIT_STACK,&sLimit))
      cerr<<"setrlimit RLIMIT_STACK error" <<endl,exit(-1);
    //ulimit -c unlimited
    struct rlimit cLimit={
      .rlim_cur=RLIM_INFINITY,
      .rlim_max=RLIM_INFINITY,
    };
    if(setrlimit(RLIMIT_CORE,&cLimit))
      cerr<<"setrlimit RLIMIT_CORE error" <<endl,exit(-1);    
  }
  string l4type;
  uint16_t l4port;
  {
    IniParser ini(argv[1]);
    {//server
      l4type=ini["l4type"];
      if(l4type!="tcp"&&l4type!="udp"){
	DEBUG_MSG("error: unknown l4type("<<l4type<<")"<<endl);
	return -1;
      }
      l4port=stoi(ini["l4port"]);
      g_pubkey=ini["pubkey"];    
      g_privkey=ini["privkey"];
      if(g_pubkey.empty()||g_privkey.empty()){
	DEBUG_MSG("error: rsa key error");
	return -1;
      }
      g_keypub=RSA::str2key(g_pubkey);
      g_keypriv=RSA::str2key(g_privkey);
      DEBUG_MSG("pubkey:\n"<<g_pubkey<<"\nprivkey:\n"<<g_privkey);
    }
    if(!ini["rlimit_cpu_limit_ini_path"].empty()){//cpu limit
      IniParser cpuLimitIni(ini["rlimit_cpu_limit_ini_path"]);
      if(!cpuLimitIni["default"].empty())
	g_cpuUsecs[ResourceLimit::Cpu::LIMIT_KEY::DEFAULT]=stol(cpuLimitIni["default"]);
    }
    if(!ini["rlimit_qos_limit_ini_path"].empty()){//qos limit
      IniParser qosLimitIni(ini["rlimit_qos_limit_ini_path"]);
      if(!qosLimitIni["in_min"].empty())
	s_qos_in_min=stoul(qosLimitIni["in_min"]);
      if(!qosLimitIni["in_rate"].empty())    
	s_qos_in_rate=stoul(qosLimitIni["in_rate"]);
      if(!qosLimitIni["out_min"].empty())    
	s_qos_out_min=stoul(qosLimitIni["out_min"]);
      if(!qosLimitIni["out_rate"].empty())    
	s_qos_out_rate=stoul(qosLimitIni["out_rate"]);
    }
    if(!ini["rlimit_freq_limit_ini_path"].empty()){//freq limit
      IniParser freqLimitIni(ini["rlimit_freq_limit_ini_path"]);
      if(!freqLimitIni["limit_signup"].empty()){
	g_rules[Frequency::DIMENSION_SIGN_UP].limit=stoi(freqLimitIni["limit_signup"]);
      }
      if(!freqLimitIni["msec_signup"].empty()){
	g_rules[Frequency::DIMENSION_SIGN_UP].msec=stol(freqLimitIni["msec_signup"]);
      }
      if(!freqLimitIni["limit_login"].empty()){
	g_rules[Frequency::DIMENSION_LOGIN].limit=stoi(freqLimitIni["limit_login"]);
      }
      if(!freqLimitIni["msec_login"].empty()){
	g_rules[Frequency::DIMENSION_LOGIN].msec=stol(freqLimitIni["msec_login"]);
      }
      if(!freqLimitIni["limit_user_send_msg"].empty()){
	g_rules[Frequency::DIMENSION_USER_SEND_MSG].limit=stoi(freqLimitIni["limit_user_send_msg"]);
      }
      if(!freqLimitIni["msec_user_send_msg"].empty()){
	g_rules[Frequency::DIMENSION_USER_SEND_MSG].msec=stol(freqLimitIni["msec_user_send_msg"]);
      }
      if(!freqLimitIni["limit_user_req_msg"].empty()){
	g_rules[Frequency::DIMENSION_USER_REQ_MSG].limit=stoi(freqLimitIni["limit_user_req_msg"]);
      }
      if(!freqLimitIni["msec_user_req_msg"].empty()){
	g_rules[Frequency::DIMENSION_USER_REQ_MSG].msec=stoul(freqLimitIni["msec_user_req_msg"]);
      }
      if(!freqLimitIni["limit_req_msg_users"].empty()){
	g_rules[Frequency::DIMENSION_REQ_MSG_USERS].limit=stoi(freqLimitIni["limit_req_msg_users"]);
      }
      if(!freqLimitIni["msec_req_msg_users"].empty()){
	g_rules[Frequency::DIMENSION_REQ_MSG_USERS].msec=stoul(freqLimitIni["msec_req_msg_users"]);
      }            
    }
    {//resource
      if(ini["resource_ini_path"].empty()){
	DEBUG_MSG("error: resource_ini_path empty!");
	return -1;
      }
      g_resource=new Resource(ini["resource_ini_path"]);
    }
    if(!ini["rlimit_size_limit_ini_path"].empty()){//size limit
      IniParser sizeLimitIni(ini["rlimit_size_limit_ini_path"]);
      auto res=sizeLimitIni.get_result();
      for(auto iter=res.begin();iter!=res.end();++iter)
	g_sizeLimit.insert(iter->first,stol(iter->second));
    }
  }  
  Socket sock(l4type=="tcp"?SOCK_STREAM:SOCK_DGRAM,l4port);
  if(sock.fd<0)return -1;
  signal(SIGPIPE,SIG_IGN);  
  l4type=="tcp"?tcpLoop(sock):udpLoop(sock);
}
