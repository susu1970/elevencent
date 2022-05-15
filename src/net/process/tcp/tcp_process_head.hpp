#ifndef _ELEVENCENT_PROCESS_HEAD_HPP_
#define _ELEVENCENT_PROCESS_HEAD_HPP_
#include"process.h"
#include"protocol.hpp"
#include<arpa/inet.h>
namespace elevencent{
  void*handleInProcessHead(void*arg){
    using namespace std;
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    DEBUG_MSG("fd: "<<conn->fd);    
    while(1){
      int left=sizeof(TcpProtocolS::Head)-ctx->offIn;
      int n;    
      while(left>0&&(n=conn->read(ctx->bufIn+ctx->offIn,left))>0){
	left-=n;
	ctx->offIn+=n;
      }
      if(left>0){
	if(n==-1){
	  if(errno==EAGAIN||errno==EWOULDBLOCK){
	    ctx->retIn=TcpProcessContext::RETCODE_IN::OK;
	    goto ret;
	  }
	  if(errno==EINTR)
	    continue;
	  ctx->retIn=TcpProcessContext::RETCODE_IN::CLOSE;
	  goto ret;
	}
	ctx->retIn=TcpProcessContext::RETCODE_IN::CLOSE;
	goto ret;
      }
      TcpProtocolS::Head head(ctx->bufIn);
      ctx->offIn=0;
      head.type=TcpProtocolS::Head::TYPE::REQUEST_PUBKEY;
      switch(head.type){
      case TcpProtocolS::Head::TYPE::REQUEST_PUBKEY:
	if(!n){
	  ctx->retIn=TcpProcessContext::RETCODE_IN::CLOSE;
	  goto ret;
	}
	if(n==-1){
	  if(errno!=EAGAIN&&errno!=EWOULDBLOCK&&errno!=EINTR){
	    ctx->retIn=TcpProcessContext::RETCODE_IN::CLOSE;
	    goto ret;
	  }
	}
	ctx->stateInCb=TcpProcessContext::STATE_IN::REQUEST_PUBKEY;
	ctx->retIn=TcpProcessContext::RETCODE_IN::NEXT;      
	goto ret;
      default:
	ctx->retIn=TcpProcessContext::RETCODE_IN::CLOSE;
	goto ret;
      }
    }
  ret:
    return arg;
  }
  TCP_PROCESS_REGISTE_IN(head,TcpProcessContext::STATE_IN::HEAD,handleInProcessHead);
}
#endif
