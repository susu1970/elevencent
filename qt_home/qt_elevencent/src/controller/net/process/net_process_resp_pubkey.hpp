#ifndef NET_PROCESS_RESP_PUBKEY_HPP_
#define  NET_PROCESS_RESP_PUBKEY_HPP_

#include"net_process.h"
#include"app.h"
#include"event.h"
namespace qt_elevencent{
  class NetProcessRespPubkeyCtx{
  public:
    TcpProtocol::RespRsaPubkey rsp;
    enum STATE{
      LEN,
      PUBKEY,
    };
    char*pubkey=0;
    STATE state=LEN;
    ~NetProcessRespPubkeyCtx(){
      if(pubkey)
	delete[]pubkey;
    }
  };
  static void netProcessRespPubkey(void*arg){
    using namespace std;
    NetThread*thr=(NetThread*)arg;
    thr->m_retIn=NetModel::RETCODE::OK;
    if(thr->m_inList.empty())
      thr->m_inList.push_back(make_pair(NetModel::STATE_IN::RESP_PUBKEY,new NetModel::StateCtx(new NetProcessRespPubkeyCtx,sizeof(NetProcessRespPubkeyCtx),[](NetModel::StateCtx*ctx){
        NetProcessRespPubkeyCtx*data=(NetProcessRespPubkeyCtx*)ctx->m_data;
	if(data)
	  delete data;
      })));
    auto iter=thr->m_inList.begin();
    auto in=*iter;
    auto ctx=get<NetModel::StateCtx*>(in);
    NetProcessRespPubkeyCtx*localCtx=(NetProcessRespPubkeyCtx*)ctx->m_data;
  again_state:
    switch(localCtx->state){
    case NetProcessRespPubkeyCtx::STATE::LEN:{
      int left=sizeof(localCtx->rsp)-ctx->m_dealLen;
      int n=thr->read((char*)&localCtx->rsp+ctx->m_dealLen,left);
      ctx->m_dealLen+=n;
      left-=n;
      if(left)
	return;
      localCtx->rsp.n2h();
      if(localCtx->rsp.len>4000){
	thr->m_retIn|=NetModel::RETCODE::INVALID_PACKET;
	return;
      }
      localCtx->pubkey=new char[localCtx->rsp.len+1];
      localCtx->pubkey[localCtx->rsp.len]=0;
      localCtx->state=NetProcessRespPubkeyCtx::STATE::PUBKEY;
      ctx->m_dealLen=0;
      goto again_state;
    }
      break;
    case NetProcessRespPubkeyCtx::STATE::PUBKEY:{
      int left=localCtx->rsp.len-ctx->m_dealLen;
      int n=thr->read(localCtx->pubkey+ctx->m_dealLen,left);
      ctx->m_dealLen+=n;
      if(left-=n)
	return;
      thr->m_curHost.rsaKeypub=RSA::str2key(localCtx->pubkey);
      thr->m_inList.erase(iter);
      delete ctx;    
      thr->m_retIn|=NetModel::RETCODE::AGAIN_IN;
      thr->m_stateIn=NetModel::STATE_IN::START;
      //      App::getInstance()->postEvent(EVENT_TYPE_CMD_VIEW_OUTPUT,new CmdViewOutputArg(CMD_VIEW_CONTENT_IDX::CONTENT,QString::fromStdString(string("pubkey:")+thr->m_curHost.rsaKeypub.str+"\n")));
    }
      break;
    default:
      break;
    }
  }
  NET_PROCESS_REGISTE_IN(resp_pubkey,NetModel::STATE_IN::RESP_PUBKEY,netProcessRespPubkey);
}

#endif
