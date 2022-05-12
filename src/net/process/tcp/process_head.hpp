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
    while(1){
      int left=sizeof(TcpProtocolS::Head)-ctx->off;
      int n;    
      while(left>0&&(n=conn->read(ctx->buf+ctx->off,left))>0){
	left-=n;
	ctx->off+=n;
      }
      if(left>0){
	if(n==-1){
	  if(errno==EAGAIN||errno==EWOULDBLOCK){
	    ctx->retcode=TcpProcessContext::RETCODE::OK;	  
	    goto ret;
	  }
	  if(errno==EINTR)
	    continue;
	  ctx->retcode=TcpProcessContext::RETCODE::CLOSE;
	  goto ret;
	}
	ctx->retcode=TcpProcessContext::RETCODE::CLOSE;
	goto ret;
      }
      TcpProtocolS::Head head(ctx->buf);
      DEBUG_MSG("fd: "<<conn->fd<<"\nhead.version: "<<head.version<<"\nhead.type: "<<head.type<<endl);
      ctx->off=0;    
      switch(head.type){
      case TcpProtocolS::Head::TYPE::REQUEST_PUBKEY:
	if(!n){
	  ctx->retcode=TcpProcessContext::RETCODE::CLOSE;
	  goto ret;
	}
	if(n==-1){
	  if(errno!=EAGAIN&&errno!=EWOULDBLOCK&&errno!=EINTR){
	    ctx->retcode=TcpProcessContext::RETCODE::CLOSE;
	    goto ret;
	  }
	}
	ctx->stateCb=TcpProcessContext::STATE::REQUEST_PUBKEY;
	ctx->retcode=TcpProcessContext::RETCODE::NEXT_HANDLE;      
	goto ret;
      default:
	ctx->retcode=TcpProcessContext::RETCODE::CLOSE;
	goto ret;
      }
    }
  ret:
    return arg;
  }
  void handleInCbProcessHead(void*arg){
    using namespace std;
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    switch(ctx->retcode){
    case TcpProcessContext::RETCODE::CLOSE:
      delete conn;
      return;
    case TcpProcessContext::RETCODE::OK:
      return;
    case TcpProcessContext::RETCODE::NEXT_HANDLE:
      ctx->state=ctx->stateCb;
      conn->handleInCb(conn->handleIn(arg));
      return;
    default:
      delete conn;
      return;
    }
  }
  TCP_PROCESS_REGISTE(ProcessHead,TcpProcessContext::STATE::HEAD,handleInProcessHead,handleInCbProcessHead,0,0);
}
#endif
