#include"global.h"
#include"thread_pool.h"
#include"connection.h"
#include"epoll.h"
#include"process.h"
#include"rsa.h"

#include<INIReader.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<cstring>
#include<string.h>
#include<string>
#include"load_tcp_process.h"

using namespace std;
using namespace elevencent;
RSA::Key g_rsaPubKey("0082010etehrkr6goj4kpbgivsntorkrdfkc3il9rp8r1ab1q57i27v6abkemfjdm1jsde02oj81lrhc9kqk93s70t87hgec15pc3gslnpnpbrkjscl8c7vqvrr45mevd3hsfa647qig9gipmkjp1hl2i10kt48p6d941delp7v0uibenojo264opprtqnvpf0m6erujl1df2n2ng78qmaa976knn2tjhodnd1kdmqen4a40fijo4l7s45873gi8s2u6bvcpmmk5vbqie0m3n6og8v8ruhlentnbjvtpn371tbd75tkjqg4ga9cpnkdpp62rn472fi8eo9011n17g2crlqh8h8oppfcu1sa7aqb6o7fdkcb4bcj1joakge7u0afv918recl55tu0svn6pgqe10e7g8odjp6egonvbf8ku2b9vhqgj1ahdg7vtc56m9u6bsvm0skevb1sk4d2bla47me0sfp1tphibvivhb9b8gb8geko29ps4lb58c4u0p2c02c64rueu9o8thsa4cevo297trvustbqtkucj6d0lsfmu4bdt9smeeu3hh1j1gakgc3a3c2d1gta0tnba5gv5gfsu4d4pofrjmgrv4hqpq27jjjqr42trr6saqn0blebvslmb6s1ggciug7viqaompoi26vff0uh28pgj6vgn5u1i611np746fs6k6eqhvn6mm63ochniunjlt777frs8lut7dcg1a0jd9fu3dckqta0clhhgajvmid5ms1feso3f130tnfcvr5da3910474v8rkgj9vlfre8qamv6lnurj04nfvdm5k9000042001");
RSA::Key g_rsaPrvKey("0082010etehrkr6goj4kpbgivsntorkrdfkc3il9rp8r1ab1q57i27v6abkemfjdm1jsde02oj81lrhc9kqk93s70t87hgec15pc3gslnpnpbrkjscl8c7vqvrr45mevd3hsfa647qig9gipmkjp1hl2i10kt48p6d941delp7v0uibenojo264opprtqnvpf0m6erujl1df2n2ng78qmaa976knn2tjhodnd1kdmqen4a40fijo4l7s45873gi8s2u6bvcpmmk5vbqie0m3n6og8v8ruhlentnbjvtpn371tbd75tkjqg4ga9cpnkdpp62rn472fi8eo9011n17g2crlqh8h8oppfcu1sa7aqb6o7fdkcb4bcj1joakge7u0afv918recl55tu0svn6pgqe10e7g8odjp6egonvbf8ku2b9vhqgj1ahdg7vtc56m9u6bsvm0skevb1sk4d2bla47me0sfp1tphibvivhb9b8gb8geko29ps4lb58c4u0p2c02c64rueu9o8thsa4cevo297trvustbqtkucj6d0lsfmu4bdt9smeeu3hh1j1gakgc3a3c2d1gta0tnba5gv5gfsu4d4pofrjmgrv4hqpq27jjjqr42trr6saqn0blebvslmb6s1ggciug7viqaompoi26vff0uh28pgj6vgn5u1i611np746fs6k6eqhvn6mm63ochniunjlt777frs8lut7dcg1a0jd9fu3dckqta0clhhgajvmid5ms1feso3f130tnfcvr5da3910474v8rkgj9vlfre8qamv6lnurj04nfvdm5k900819bd54fsoduh1l4tvf6jha24q2q7dufkhthdpikp7mc57cjnvsghcuadp4jtolh48ssatmc67jmh3lr1l6g423lh9tlgveiiqeucissumjg70rp7oo137mc5m67f629teaknguck5mfosfr7mac63g20gl3hm8djg2bhi9icq15nfbsb393ej4lo4k78gdrs01sb6a17p4a3cilcgqb3gpputau4n6niesmlnqcgktfntdc8icuborutkrhqcldn5kdn8gim2d33b9o039qo1ab4n3uhfo8r4a4j4f2bdvdscilarjuil97jle3lfv1994dh76mkl0m1507v2lon6h1od69594259jophrpa47u6qnlcu9kalsgbbm7aas5tfipd8735v48rd6ku1ql8melic0mfvcempun7bi34so466doc29dmu8parmbpggh26av62mdvvrhmi8vft05jtei14es0bujmf871am0710nrdt7vh0q76l32r8a84dvjipcer5jfu5v1t30rn5djar8c74q4e4de5lmc2a3p5j0l5k1jlrp1k10m0l5tn79iiam4v4oh7cbb8q4o2hg7l19mikvpgvds16uuq6btgk4157acvokl5q2hi1kr8hjo9igq4gmiv5l76pnh621rns25u25oteq0oe3pv3j9igcg71v5sbcc6kp6lun46dqodi6a0bobrkbd5ggs2l04bdjuef3ec3s5vrmacscehttbk48j3njd4cf8kckegdu5kinikvrtqhs8phk1c9it5j22u5ipksr3ndcljbv45ravkt4hp6ro1");    
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
  l4type=="tcp"?tcpLoop(sock):udpLoop(sock);
}
