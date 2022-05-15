#ifndef _ELEVENCENT_TCP_PROCESS_REQUEST_PUBKEY_H_
#define _ELEVENCENT_TCP_PROCESS_REQUEST_PUBKEY_H_

#include"process.h"
#include"protocol.hpp"
#include<arpa/inet.h>

namespace elevencent{
  void*handleInProcessRequestPubkey(void*arg){
    using namespace std;
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    DEBUG_MSG("fd: "<<conn->fd<<endl);
    int n;
    int left=ctx->rsaPubkey->str.size();
    while(1){
      while(left>0&&(n=conn->write(ctx->rsaPubkey->str.c_str()+ctx->offOut,left))>0){
	left-=n;
	ctx->offOut+=n;
      }
      if(left==0){
	ctx->offOut=0;		
	if(n==-1){
	  if(errno!=EAGAIN&&errno!=EWOULDBLOCK&&errno!=errno==EINTR){
	    ctx->retIn=TcpProcessContext::RETCODE_IN::CLOSE;
	    goto ret;	    
	  }
	  ctx->retIn=TcpProcessContext::RETCODE_IN::NEXT;
	  ctx->stateInCb=TcpProcessContext::STATE_IN::START;
	  goto ret;
	}
	ctx->retIn=TcpProcessContext::RETCODE_IN::NEXT;
	ctx->stateInCb=TcpProcessContext::STATE_IN::START;
	goto ret;	
      }
      if(errno==EINTR)continue;
      if(errno==EAGAIN||errno==EWOULDBLOCK){
	struct epoll_event ev;
	ev.data.ptr=conn;
	ev.events=EPOLLOUT|EPOLLET|EPOLLIN;
	if(unlikely(ctx->ep->ctl(EPOLL_CTL_MOD,conn->fd,&ev)==-1)){
	  ctx->retIn=TcpProcessContext::RETCODE_IN::CLOSE;
	  goto ret;
	}
	ctx->retIn=TcpProcessContext::RETCODE_IN::NEXT;
	ctx->stateInCb=TcpProcessContext::STATE_IN::START;
      }
      ctx->retIn=TcpProcessContext::RETCODE_IN::CLOSE;
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
    int left=ctx->rsaPubkey->str.size();
    DEBUG_MSG("fd: "<<conn->fd<<", out");
    while(1){
      
    }
  ret:
    return arg;
  }
  TCP_PROCESS_REGISTE_IN(request_pubkey,TcpProcessContext::STATE_IN::REQUEST_PUBKEY,handleInProcessRequestPubkey);
  TCP_PROCESS_REGISTE_OUT(request_pubkey,TcpProcessContext::STATE_OUT::REQUEST_PUBKEY,handleOutProcessRequestPubkey);
}


#endif
