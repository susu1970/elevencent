#ifndef _ELEVENCENT_TCP_PROCESS_OUT_START_HPP_
#define _ELEVENCENT_TCP_PROCESS_OUT_START_HPP_

#include"process.h"
#include"protocol.hpp"
#include<arpa/inet.h>
namespace elevencent{
  void*handleOutProcessStart(void*arg){
    using namespace std;
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    ctx->outLock.lock();
    if(ctx->outList.empty()){
      ctx->outLock.unlock();
      ctx->retOut=TcpProcessContext::RETCODE::OK;
      goto ret;
    }
    ctx->stateOutCb=get<TcpProcessContext::STATE_OUT>(ctx->outList.front());
    ctx->outLock.unlock();    
    ctx->retOut=TcpProcessContext::RETCODE::NEXT;
  ret:
    return arg;
  }
  TCP_PROCESS_REGISTE_OUT(start,TcpProcessContext::STATE_OUT::START,handleOutProcessStart);  
}
#endif
