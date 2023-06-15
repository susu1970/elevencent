#ifndef TCP_PROCESS_RESP_PUBKEY_HPP_
#define TCP_PROCESS_RESP_PUBKEY_HPP_

#include"process.h"
#include"global.h"
#include"resource_global.h"
#include"resource.h"
#include"rsa.h"

namespace elevencent{
  class TcpProcessRespPubkeyCtx{
  public:
    int offIn=0;
    TcpProtocol::RespPubkey rp;
    char*pubkey=0;
    enum STATE{
      LEN,
      PUBKEY,
    };
    STATE state=LEN;
    ~TcpProcessRespPubkeyCtx(){
      if(pubkey)
	delete[]pubkey;
    }
  };
  void*handleInProcessRespPubkey(void*arg){
    using namespace std;
    using namespace ResourceLimit;
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    ctx->retIn=TcpProcessContext::RETCODE::OK;    
    TcpProcessRespPubkeyCtx*localCtx=(TcpProcessRespPubkeyCtx*)ctx->ctxIn[TcpProcessContext::STATE_IN::RESP_PUBKEY];
    if(!localCtx){
      ctx->ctxIn[TcpProcessContext::STATE_IN::RESP_PUBKEY]=localCtx=new TcpProcessRespPubkeyCtx;
      ctx->registeOnDestroyFunc(TcpProcessContext::STATE_IN::RESP_PUBKEY,[](void*arg){
	TcpProcessContext*ctx=(TcpProcessContext*)arg;
        TcpProcessRespPubkeyCtx*localCtx=(TcpProcessRespPubkeyCtx*)ctx->ctxIn[TcpProcessContext::STATE_IN::RESP_PUBKEY];
	if(localCtx)
	  delete localCtx;
	ctx->ctxIn.erase(TcpProcessContext::STATE_IN::RESP_PUBKEY);
      });      
    }
  state_again:
    switch(localCtx->state){
    case TcpProcessRespPubkeyCtx::STATE::LEN:{
      int left=sizeof(localCtx->rp)-localCtx->offIn;
      int err;
      int n=conn->read((char*)&localCtx->rp+localCtx->offIn,left,&err);
      left-=n;
      localCtx->offIn+=n;
      if(err&Connection::ERRNO_RLIMIT_QOS)
	ctx->retIn|=TcpProcessContext::RETCODE::RLIMIT_QOS;	
      if(err&Connection::ERRNO_CLOSE){
	ctx->retIn|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
	return arg;
      }
      if(left>0)
	return arg;
      localCtx->rp.ntoh();
      if(localCtx->rp.len>g_sizeLimit["tcp_process_resp_pubkey"]){
	DEBUG_PRETTY_MSG("invalid pubkey length, len: "<<localCtx->rp.len<<", limit: "<<g_sizeLimit["tcp_process_resp_pubkey"]);
	ctx->retIn|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
	return arg;
      }
      localCtx->pubkey=new char[localCtx->rp.len+1];
      localCtx->pubkey[localCtx->rp.len]=0;
      localCtx->offIn=0;
      localCtx->state=TcpProcessRespPubkeyCtx::STATE::PUBKEY;
      goto state_again;
    }
      break;
    case TcpProcessRespPubkeyCtx::STATE::PUBKEY:{
      int left=localCtx->rp.len-localCtx->offIn;
      int err;
      int n=conn->read((char*)localCtx->pubkey+localCtx->offIn,left,&err);
      left-=n;
      localCtx->offIn+=n;
      if(err&Connection::ERRNO_RLIMIT_QOS)
	ctx->retIn|=TcpProcessContext::RETCODE::RLIMIT_QOS;
      if(err&Connection::ERRNO_CLOSE){
	ctx->retIn|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
	return arg;	  
      }
      if(left>0)
	return arg;
      ctx->pubkey=localCtx->pubkey;
      delete localCtx;
      ctx->ctxIn.erase(TcpProcessContext::STATE_IN::RESP_PUBKEY);
      ctx->stateIn=TcpProcessContext::STATE_IN::START;
      ctx->retIn|=TcpProcessContext::RETCODE::NEXT;
    }
      break;
    default:
      break;
    }
    return arg;
  }
  TCP_PROCESS_REGISTE_IN(resp_pubkey,TcpProcessContext::STATE_IN::RESP_PUBKEY,handleInProcessRespPubkey);  
}

#endif
