#ifndef _ELEVENCENT_PROCESS_HEAD_HPP_
#define _ELEVENCENT_PROCESS_HEAD_HPP_
#include"process.h"
#include"protocol.hpp"
#include<arpa/inet.h>
#include"global.h"
namespace elevencent{
  class TcpProcessHeadContext{
  public:
    uint8_t offIn=0;
    TcpProtocol::Head headBuf;    
  };
  void*handleInProcessHead(void*arg){
    using namespace std;
    using namespace elevencent;        
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    ctx->retIn=TcpProcessContext::RETCODE::OK;
    TcpProcessHeadContext*headCtx=(TcpProcessHeadContext*)ctx->ctxIn[TcpProcessContext::STATE_IN::HEAD];
    if(!headCtx){
      ctx->ctxIn[TcpProcessContext::STATE_IN::HEAD]=headCtx=new TcpProcessHeadContext;      
      ctx->registeOnDestroyFunc(TcpProcessContext::STATE_IN::HEAD,[](void*arg){
	TcpProcessContext*ctx=(TcpProcessContext*)arg;
	TcpProcessHeadContext*headCtx=(TcpProcessHeadContext*)ctx->ctxIn[TcpProcessContext::STATE_IN::HEAD];
	if(headCtx)
	  delete headCtx;
	ctx->ctxIn.erase(TcpProcessContext::STATE_IN::HEAD);
      });
    }
    int left=sizeof(TcpProtocol::Head)-headCtx->offIn;
    int err;
    int n=conn->read((char*)&headCtx->headBuf+headCtx->offIn,left,&err);
    left-=n;
    headCtx->offIn+=n;
    if(err&Connection::ERRNO_RLIMIT_QOS)
      ctx->retIn|=TcpProcessContext::RETCODE::RLIMIT_QOS;
    if(err&Connection::ERRNO_CLOSE){
      ctx->retIn|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
      return arg;
    }
    if(left>0)
      return arg;
    headCtx->offIn=0;
    headCtx->headBuf.ntoh();
    ctx->retIn|=TcpProcessContext::RETCODE::IN_AGAIN;    
    switch(headCtx->headBuf.type){
    case TcpProtocol::Head::TYPE::REQUEST_PUBKEY:
      ctx->stateIn=TcpProcessContext::STATE_IN::REQUEST_PUBKEY;
      break;
    case TcpProtocol::Head::TYPE::SIGN_UP:
      ctx->stateIn=TcpProcessContext::STATE_IN::SIGN_UP;
      break;
    case TcpProtocol::Head::TYPE::LOGIN:
      ctx->stateIn=TcpProcessContext::STATE_IN::LOGIN;      
      break;
    case TcpProtocol::Head::TYPE::RESP_PUBKEY:
      ctx->stateIn=TcpProcessContext::STATE_IN::RESP_PUBKEY;      
      break;
    case TcpProtocol::Head::TYPE::USER_SEND_MSG:
      ctx->stateIn=TcpProcessContext::STATE_IN::USER_SEND_MSG;      
      break;
    case TcpProtocol::Head::TYPE::USER_REQ_MSG:
      ctx->stateIn=TcpProcessContext::STATE_IN::USER_REQ_MSG;      
      break;
    case TcpProtocol::Head::TYPE::REQ_MSG_USERS:
      ctx->stateIn=TcpProcessContext::STATE_IN::REQ_MSG_USERS;      
      break;    
    default:
      ctx->retIn|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
      break;
    }
    return arg;
  }
  TCP_PROCESS_REGISTE_IN(head,TcpProcessContext::STATE_IN::HEAD,handleInProcessHead);
}
#endif
