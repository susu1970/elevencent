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
    ctx->retOut=TcpProcessContext::RETCODE::OK;
    if(ctx->outStartTurn++%2){
      ctx->stateOut=TcpProcessContext::STATE_OUT::FRAG;
      ctx->retOut|=TcpProcessContext::RETCODE::OUT_AGAIN;      
      return arg;
    }    
    if(!ctx->outList.size()){
      if(!ctx->outFragList.size())
	return arg;
      ctx->retOut|=TcpProcessContext::RETCODE::OUT_AGAIN;            
      ctx->stateOut=TcpProcessContext::STATE_OUT::FRAG;
      return arg;      
    }
    ctx->stateOut=ctx->outList.front()->stateOut;
    ctx->retOut|=TcpProcessContext::RETCODE::OUT_AGAIN;          
    return arg;
  }
  TCP_PROCESS_REGISTE_OUT(start,TcpProcessContext::STATE_OUT::START,handleOutProcessStart);
  class TcpProcessOutStartFragCtx{
  public:
    TcpProcessContext::List::Node*prevOutFragNode=0;
    ~TcpProcessOutStartFragCtx(){
      if(prevOutFragNode)
	delete prevOutFragNode;
    }
  };
  void*handleOutProcessOutFrag(void*arg){
    using namespace std;
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    ctx->retOut=TcpProcessContext::RETCODE::NEXT;
    ctx->stateOut=TcpProcessContext::STATE_OUT::START;
    TcpProcessOutStartFragCtx*localCtx=(TcpProcessOutStartFragCtx*)ctx->ctxOut[TcpProcessContext::STATE_OUT::FRAG];
    if(!localCtx){
      ctx->ctxOut[TcpProcessContext::STATE_OUT::FRAG]=localCtx=new TcpProcessOutStartFragCtx;      
      ctx->registeOnDestroyFunc(TcpProcessContext::STATE_OUT::FRAG,[](void*arg){
	TcpProcessContext*ctx=(TcpProcessContext*)arg;
	TcpProcessOutStartFragCtx*localCtx=(TcpProcessOutStartFragCtx*)ctx->ctxOut[TcpProcessContext::STATE_OUT::FRAG];
	if(localCtx)
	  delete localCtx;
	ctx->ctxOut.erase(TcpProcessContext::STATE_OUT::FRAG);
      });
    }
    if(localCtx->prevOutFragNode){
      if(ctx->outList.push_back(localCtx->prevOutFragNode))
	localCtx->prevOutFragNode=0;
      return arg;            	      
    }
    if(!ctx->outFragList.size())
      return arg;
    auto outFragNode=ctx->outFragList.front();
    for(uint32_t turn=ctx->outFragTurn++%ctx->outFragList.size();turn>0;({outFragNode=outFragNode->next;--turn;}));
    TcpProcessContext::OutFragCtx*fragCtx=(TcpProcessContext::OutFragCtx*)outFragNode->ctx;
    int len;            
    if((len=fragCtx->read(fragCtx,0,0))<=0){
      ctx->outFragList.rm(outFragNode);
      delete outFragNode;
      return arg;
    }
    char*buf=new char[len];
    int n=fragCtx->read(fragCtx,buf,len);
    if(n<=0){
      ctx->outFragList.rm(outFragNode);
      delete[]buf;
      delete outFragNode;
      return arg;
    }
    TcpProcessContext::DefaultOutCtx*outctx=new TcpProcessContext::DefaultOutCtx(buf,n,[](TcpProcessContext::DefaultOutCtx*ctx){
      delete[](char*)ctx->m_data;
    });
    TcpProcessContext::List::Node*outNode=new TcpProcessContext::List::Node(TcpProcessContext::STATE_OUT::SEND_FIFO_GROUP,outctx,[](TcpProcessContext::List::Node*node){
      delete(TcpProcessContext::DefaultOutCtx*)node->ctx;
    });
    localCtx->prevOutFragNode=outNode;
    if(ctx->outList.push_back(localCtx->prevOutFragNode))
      localCtx->prevOutFragNode=0;      
    return arg;
  }
  TCP_PROCESS_REGISTE_OUT(out_frag,TcpProcessContext::STATE_OUT::FRAG,handleOutProcessOutFrag);
}
#endif
