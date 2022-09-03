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
    ctx->outLock.lock();
    ctx->outList.push_back(std::make_pair(TcpProcessContext::STATE_OUT::REQUEST_PUBKEY,new TcpProcessRequestPubkeyContext));
    ctx->outLock.unlock();
    ctx->retIn=TcpProcessContext::RETCODE::EP_OUT|TcpProcessContext::RETCODE::EP_IN;
    ctx->stateIn=TcpProcessContext::STATE_IN::START;
    return arg;
  }
  void*handleOutProcessRequestPubkey(void*arg){
    using namespace std;
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    ctx->outLock.lock();
    TcpProcessRequestPubkeyContext*localCtx=(TcpProcessRequestPubkeyContext*)get<void*>(ctx->outList.front());
    ctx->outLock.unlock();   
    int n;
    int left=gk_rsaPubkey.str.size()-localCtx->offOut;
    while(1){
      while(left>0&&(n=conn->write(gk_rsaPubkey.str.c_str()+localCtx->offOut,left))>0){
	left-=n;
	localCtx->offOut+=n;
      }
      //      DEBUG_MSG("left("<<left<<")!=0\nfd: "<<conn->fd<<"\noffOut: "<<localCtx->offOut<<"\nn: "<<n<<"\nerrno: "<<errno<<"\nerrstr: "<<strerror(errno)<<endl);
      if(left==0){
	delete localCtx;
	ctx->outLock.lock();
	ctx->outList.pop_front();
	if(ctx->outList.empty()){
	  ctx->outLock.unlock();
	  ctx->retOut=TcpProcessContext::RETCODE::OK;
	  ctx->stateOut=TcpProcessContext::STATE_OUT::START;
	  goto ret;
	}
	ctx->stateOutCb=get<TcpProcessContext::STATE_OUT>(ctx->outList.front());
	ctx->outLock.unlock();
	ctx->retOut=TcpProcessContext::RETCODE::NEXT;
	goto ret;	
      }
      if(errno==EINTR)
	continue;
      if(errno==EAGAIN||errno==EWOULDBLOCK){
	ctx->retOut=TcpProcessContext::RETCODE::OK;
	goto ret;
      }
      ctx->retOut=TcpProcessContext::RETCODE::CLOSE;
      goto ret;
    }
  ret:
    return arg;
  }
  TCP_PROCESS_REGISTE_IN(request_pubkey,TcpProcessContext::STATE_IN::REQUEST_PUBKEY,handleInProcessRequestPubkey);
  TCP_PROCESS_REGISTE_OUT(request_pubkey,TcpProcessContext::STATE_OUT::REQUEST_PUBKEY,handleOutProcessRequestPubkey);
}


#endif
