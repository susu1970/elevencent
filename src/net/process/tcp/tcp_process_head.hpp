#ifndef _ELEVENCENT_PROCESS_HEAD_HPP_
#define _ELEVENCENT_PROCESS_HEAD_HPP_
#include"process.h"
#include"protocol.hpp"
#include<arpa/inet.h>
namespace elevencent{
  class TcpProcessHeadContext{
  public:
    uint8_t offIn;
    TcpProtocolS::Head headBuf;    
    std::function<void(void*)>onDestroy;
    TcpProcessHeadContext(std::function<void(void*)>&&onDestroy_=std::forward<std::function<void(void*)>>([](void*){})):offIn(0),onDestroy(std::forward<std::function<void(void*)>>(onDestroy_)){}
    ~TcpProcessHeadContext(){
      onDestroy(this);
    }
  };
  void*handleInProcessHead(void*arg){
    using namespace std;
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    TcpProcessHeadContext*headCtx=(TcpProcessHeadContext*)ctx->ctxIn[TcpProcessContext::STATE_IN::HEAD];
    if(!headCtx){
      ctx->ctxIn[TcpProcessContext::STATE_IN::HEAD]=headCtx=new TcpProcessHeadContext();
      ctx->registeOnDestroyFunc([](void*arg){
	TcpProcessContext*ctx=(TcpProcessContext*)arg;
	TcpProcessHeadContext*headCtx=(TcpProcessHeadContext*)ctx->ctxIn[TcpProcessContext::STATE_IN::HEAD];
	if(headCtx){
	  delete headCtx;
	  ctx->ctxIn.erase(TcpProcessContext::STATE_IN::HEAD);
	}
      });
    }
    while(1){
      int left=sizeof(headCtx->headBuf)-headCtx->offIn;
      int n;    
      while(left>0&&(n=conn->read(&headCtx->headBuf+headCtx->offIn,left))>0){
	left-=n;
	headCtx->offIn+=n;
      }
      DEBUG_MSG("fd: "<<conn->fd<<"\nsizeof(TcpProtocolS::Head): "<<sizeof(TcpProtocolS::Head)<<"\noffIn: "<<headCtx->offIn<<"\nleft: "<<left);    
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
      TcpProtocolS::Head head(&headCtx->headBuf);
      headCtx->offIn=0;
      head.type=TcpProtocolS::Head::TYPE::REQUEST_PUBKEY;
      switch(head.type){
      case TcpProtocolS::Head::TYPE::REQUEST_PUBKEY:
	ctx->retIn=TcpProcessContext::RETCODE_IN::NEXT;		
	ctx->stateInCb=TcpProcessContext::STATE_IN::REQUEST_PUBKEY;
	goto ret;
      case TcpProtocolS::Head::TYPE::INSERT_USER_RESOURCE:
	ctx->retIn=TcpProcessContext::RETCODE_IN::NEXT;		
	ctx->stateInCb=TcpProcessContext::STATE_IN::INSERT_USER_RESOURCE;	
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
