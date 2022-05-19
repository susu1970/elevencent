#ifndef _ELEVENCENT_TCP_PROCESS_REQUEST_PUBKEY_H_
#define _ELEVENCENT_TCP_PROCESS_REQUEST_PUBKEY_H_

#include<arpa/inet.h>

#include"process.h"
#include"protocol.hpp"

namespace elevencent{
  class TcpProcessRequestPubkeyContext{
  public:
    int offOut;
    TcpProcessRequestPubkeyContext():offOut(0){}    
  };
  void*handleInProcessRequestPubkey(void*arg){
    using namespace std;
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    TcpProcessRequestPubkeyContext*localCtx=(TcpProcessRequestPubkeyContext*)ctx->ctxIn[TcpProcessContext::STATE_IN::REQUEST_PUBKEY];
    if(!localCtx){
      ctx->ctxIn[TcpProcessContext::STATE_IN::REQUEST_PUBKEY]=localCtx=new TcpProcessRequestPubkeyContext;
      ctx->registeOnDestroyFunc([](void*arg){
	TcpProcessContext*ctx=(TcpProcessContext*)arg;
	TcpProcessRequestPubkeyContext*localCtx=(TcpProcessRequestPubkeyContext*)ctx->ctxIn[TcpProcessContext::STATE_IN::REQUEST_PUBKEY];
	if(localCtx){
	  delete localCtx;
	  ctx->ctxIn.erase(TcpProcessContext::STATE_IN::REQUEST_PUBKEY);
	}
      });
    }
    int n;
    int left=gk_rsaPubkey.str.size()-localCtx->offOut;
    while(1){
      while(left>0&&(n=conn->write(gk_rsaPubkey.str.c_str()+localCtx->offOut,left))>0){
	left-=n;
	localCtx->offOut+=n;
      }
      DEBUG_MSG("left("<<left<<")!=0\nfd: "<<conn->fd<<"\noffOut: "<<localCtx->offOut<<"\nn: "<<n<<"\nerrno: "<<errno<<"\nerrstr: "<<strerror(errno)<<endl);                        
      if(left==0){
	localCtx->offOut=0;		
	ctx->retIn=TcpProcessContext::RETCODE::NEXT;
	ctx->stateInCb=TcpProcessContext::STATE_IN::START;
	goto ret;	
      }
      if(errno==EINTR)
	continue;
      if(errno==EAGAIN||errno==EWOULDBLOCK){
	struct epoll_event ev;
	memcpy(&ev,&ctx->ev,sizeof(ev));
	ev.events|=EPOLLOUT;
	if(unlikely(ctx->ep->ctl(EPOLL_CTL_MOD,conn->fd,&ev)==-1)){
	  ctx->retIn=TcpProcessContext::RETCODE::CLOSE;
	  goto ret;
	}
	ctx->retIn=TcpProcessContext::RETCODE::NEXT;
	ctx->stateInCb=TcpProcessContext::STATE_IN::START;
	goto ret;
      }
      ctx->retIn=TcpProcessContext::RETCODE::CLOSE;
      goto ret;
    }
  ret:
    return arg;
  }
  void*handleOutProcessRequestPubkey(void*arg){
    using namespace std;
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    int n;
    int left=gk_rsaPubkey.str.size();
    DEBUG_MSG("fd: "<<conn->fd<<", out....");
  ret:
    return arg;
  }
  TCP_PROCESS_REGISTE_IN(request_pubkey,TcpProcessContext::STATE_IN::REQUEST_PUBKEY,handleInProcessRequestPubkey);
  TCP_PROCESS_REGISTE_OUT(request_pubkey,TcpProcessContext::STATE_OUT::REQUEST_PUBKEY,handleOutProcessRequestPubkey);
}


#endif
