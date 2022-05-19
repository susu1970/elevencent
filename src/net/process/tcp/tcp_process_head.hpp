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
    TcpProcessHeadContext():offIn(0){}
  };
  void*handleInProcessHead(void*arg){
    using namespace std;
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    TcpProcessHeadContext*headCtx=(TcpProcessHeadContext*)ctx->ctxIn[TcpProcessContext::STATE_IN::HEAD];
    if(!headCtx){
      ctx->ctxIn[TcpProcessContext::STATE_IN::HEAD]=headCtx=new TcpProcessHeadContext;
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
      DEBUG_MSG("left("<<left<<")!=0\nfd: "<<conn->fd<<"\noffIn: "<<headCtx->offIn<<"\nn: "<<n<<"\nerrno: "<<errno<<"\nerrstr: "<<strerror(errno)<<endl);      
      if(left>0){
	if(n==-1){
	  if(errno==EAGAIN||errno==EWOULDBLOCK){
	    ctx->retIn=TcpProcessContext::RETCODE::OK;
	    goto ret;
	  }
	  if(errno==EINTR)
	    continue;
	}
	ctx->retIn=TcpProcessContext::RETCODE::CLOSE;
	goto ret;
      }
      TcpProtocolS::Head head(&headCtx->headBuf);
      headCtx->offIn=0;
      head.type=TcpProtocolS::Head::TYPE::REQUEST_PUBKEY;
      switch(head.type){
      case TcpProtocolS::Head::TYPE::REQUEST_PUBKEY:
	ctx->retIn=TcpProcessContext::RETCODE::NEXT;
	ctx->stateInCb=TcpProcessContext::STATE_IN::REQUEST_PUBKEY;
	goto ret;
      case TcpProtocolS::Head::TYPE::INSERT_USER_RESOURCE:
	ctx->retIn=TcpProcessContext::RETCODE::NEXT;
	ctx->stateInCb=TcpProcessContext::STATE_IN::INSERT_USER_RESOURCE;	
	goto ret;
      default:
	ctx->retIn=TcpProcessContext::RETCODE::CLOSE;
	goto ret;
      }
      goto ret;
    }
  ret:
    return arg;
  }
  TCP_PROCESS_REGISTE_IN(head,TcpProcessContext::STATE_IN::HEAD,handleInProcessHead);
}
#endif
