#ifndef _ELEVENCENT_TCP_PROCESS_REQUEST_PUBKEY_H_
#define _ELEVENCENT_TCP_PROCESS_REQUEST_PUBKEY_H_

#include<arpa/inet.h>
#include<string>

#include"process.h"
#include"protocol.hpp"

namespace elevencent{
  class TcpProcessRequestPubkeyContext{
  public:
    int offOut=0;
    enum STATE{
      PUBKEY_HEAD,
      PUBKEY_CONTENT,
    };
    TcpProtocol::RsaPubkey pubkey;
    STATE state=PUBKEY_HEAD;
    TcpProcessContext::List::Node*outNode=0;
  };
  void*handleInProcessRequestPubkey(void*arg){
    using namespace std;
    using namespace elevencent;        
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    ctx->retIn=TcpProcessContext::RETCODE::OK;
    TcpProcessRequestPubkeyContext*localCtx=(TcpProcessRequestPubkeyContext*)ctx->ctxIn[TcpProcessContext::STATE_IN::REQUEST_PUBKEY];
    if(!localCtx){
      ctx->ctxIn[TcpProcessContext::STATE_IN::REQUEST_PUBKEY]=localCtx=new TcpProcessRequestPubkeyContext;
      localCtx->outNode=new TcpProcessContext::List::Node(TcpProcessContext::STATE_OUT::RESP_PUBKEY,localCtx,[](TcpProcessContext::List::Node*node){
	delete(TcpProcessRequestPubkeyContext*)node->ctx;
      });
      ctx->registeOnDestroyFunc(TcpProcessContext::STATE_IN::REQUEST_PUBKEY,[](void*arg){
	TcpProcessContext*ctx=(TcpProcessContext*)arg;
        TcpProcessRequestPubkeyContext*localCtx=(TcpProcessRequestPubkeyContext*)ctx->ctxIn[TcpProcessContext::STATE_IN::REQUEST_PUBKEY];
	if(localCtx){
	  if(localCtx->outNode)
	    delete localCtx->outNode;	    
	  else
	    delete localCtx;	  
	}
	ctx->ctxIn.erase(TcpProcessContext::STATE_IN::REQUEST_PUBKEY);
      });
    }
    if(ctx->outList.push_back(localCtx->outNode)){
      ctx->ctxIn.erase(TcpProcessContext::STATE_IN::REQUEST_PUBKEY);
      ctx->retIn|=TcpProcessContext::RETCODE::OUT_AGAIN|TcpProcessContext::RETCODE::NEXT;      
      ctx->stateIn=TcpProcessContext::STATE_IN::START;        
      return arg;
    }
    ctx->retIn|=TcpProcessContext::RETCODE::OUT_AGAIN|TcpProcessContext::RETCODE::IN_INSERT_EV;
    return arg;
  }
  void*handleOutProcessRequestPubkey(void*arg){
    using namespace std;
    using namespace elevencent;    
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    ctx->retOut=TcpProcessContext::RETCODE::OK;
    auto outNode=ctx->outList.front();
    TcpProcessRequestPubkeyContext*localCtx=(TcpProcessRequestPubkeyContext*)outNode->ctx;
  state_again:
    switch(localCtx->state){
    case TcpProcessRequestPubkeyContext::STATE::PUBKEY_HEAD:{
      int left=sizeof(localCtx->pubkey)-localCtx->offOut;
      int err;
      int n=conn->write((char*)&localCtx->pubkey+localCtx->offOut,left,&err);
      left-=n;
      localCtx->offOut+=n;
      if(err&Connection::ERRNO_RLIMIT_QOS)
	ctx->retOut|=TcpProcessContext::RETCODE::RLIMIT_QOS;
      if(err&Connection::ERRNO_CLOSE){
	ctx->retOut|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
	return arg;
      }
      if(left>0)
	return arg;
      localCtx->state=TcpProcessRequestPubkeyContext::STATE::PUBKEY_CONTENT;
      localCtx->offOut=0;
      goto state_again;
    }
      break;
    case TcpProcessRequestPubkeyContext::STATE::PUBKEY_CONTENT:{
      int left=g_pubkey.size()-localCtx->offOut;
      int err;
      int n=conn->write(g_pubkey.c_str()+localCtx->offOut,left,&err);
      left-=n;
      localCtx->offOut+=n;
      if(err&Connection::ERRNO_RLIMIT_QOS)
	ctx->retOut|=TcpProcessContext::RETCODE::RLIMIT_QOS;
      if(err&Connection::ERRNO_CLOSE){
	ctx->retOut|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
	return arg;
      }
      if(left)
	return arg;
      ctx->outList.rm(outNode);
      delete outNode;
      ctx->stateOut=TcpProcessContext::STATE_OUT::START;
      ctx->retOut|=TcpProcessContext::RETCODE::NEXT;    
      return arg;
    }
      break;
    default:
      break;
    }
    return arg;
  }
  TCP_PROCESS_REGISTE_IN(request_pubkey,TcpProcessContext::STATE_IN::REQUEST_PUBKEY,handleInProcessRequestPubkey);
  TCP_PROCESS_REGISTE_OUT(request_pubkey,TcpProcessContext::STATE_OUT::RESP_PUBKEY,handleOutProcessRequestPubkey);
}


#endif
