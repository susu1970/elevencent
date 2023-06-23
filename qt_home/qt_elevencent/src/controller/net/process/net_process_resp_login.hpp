#ifndef NET_PROCESS_RESP_LOGIN_HPP_
#define NET_PROCESS_RESP_LOGIN_HPP_

#include"net_process.h"
#include"app.h"
#include"event.h"
namespace qt_elevencent{
  class NetProcessRespLoginCtx{
  public:
    TcpProtocol::RespLogin rspLogin;
    char*token=0;
    enum STATE{
      LEN,
      TOKEN,
    };
    STATE state=LEN;
    ~NetProcessRespLoginCtx(){
      if(token)
	delete[]token;
    }
  };
  static void netProcessRespLogin(void*arg){
    using namespace std;
    NetThread*thr=(NetThread*)arg;
    thr->m_retIn=NetModel::RETCODE::OK;
    if(thr->m_inList.empty())
      thr->m_inList.push_back(make_pair(NetModel::STATE_IN::RESP_LOGIN,new NetModel::StateCtx(new NetProcessRespLoginCtx,sizeof(NetProcessRespLoginCtx),[](NetModel::StateCtx*ctx){
	NetProcessRespLoginCtx*rsp=(NetProcessRespLoginCtx*)ctx->m_data;
	if(rsp)
	  delete rsp;
      })));
    auto iter=thr->m_inList.begin();
    auto in=*iter;
    auto ctx=get<NetModel::StateCtx*>(in);
    NetProcessRespLoginCtx*localCtx=(NetProcessRespLoginCtx*)ctx->m_data;
  state_again:
    switch(localCtx->state){
    case NetProcessRespLoginCtx::STATE::LEN:{
      int left=sizeof(localCtx->rspLogin)-ctx->m_dealLen;
      int n=thr->read((char*)&localCtx->rspLogin+ctx->m_dealLen,left);
      left-=n;
      ctx->m_dealLen+=n;
      if(left)
	return;
      localCtx->rspLogin.ntoh();
      if(localCtx->rspLogin.cryptlen>4000){
	delete ctx;
	thr->m_inList.erase(iter);
	thr->m_retIn|=NetModel::RETCODE::INVALID_PACKET;
	thr->m_stateIn=NetModel::STATE_IN::START;
	return;
      }
      ctx->m_dealLen=0;
      localCtx->token=new char[localCtx->rspLogin.cryptlen+1];
      localCtx->token[localCtx->rspLogin.cryptlen]=0;
      localCtx->state=NetProcessRespLoginCtx::STATE::TOKEN;
      goto state_again;
    }
      break;
    case NetProcessRespLoginCtx::STATE::TOKEN:{
      int left=localCtx->rspLogin.cryptlen-ctx->m_dealLen;
      int n=thr->read(localCtx->token+ctx->m_dealLen,left);
      left-=n;
      ctx->m_dealLen+=n;
      if(left)
	return;
      QString str="";
      if(localCtx->rspLogin.isErr()){
	if(localCtx->rspLogin.code&TcpProtocol::ERR_RLIMIT_FREQ)
	  str="login too frequently, try again later";
	else if(localCtx->rspLogin.code&TcpProtocol::ERR_NO_PUBKEY)
	  str="login err, the server does not have our pubkey";
	else
	  str="login err";
      }else{
	size_t len=RSA::decryptWithRand16(localCtx->token,g_keypriv,&thr->m_curHost.loginToken,sizeof(thr->m_curHost.loginToken));
	if(len!=sizeof(thr->m_curHost.loginToken)){
	  str="login token err";
	}else{
	  thr->m_curHost.loginToken.ntoh();
	  thr->m_curHost.isLogin=true;
	  str="login success";
	}
      }
      thr->m_retIn|=NetModel::RETCODE::AGAIN_IN;
      thr->m_stateIn=NetModel::STATE_IN::START;
      delete ctx;
      thr->m_inList.erase(iter);
      App::getInstance()->postEvent(EVENT_TYPE_NET_ON_LOGIN_SUCCESS);
      App::getInstance()->postEvent(EVENT_TYPE_TOAST_SHOW,new ToastShowArg(str));	
    }
      break;
    default:
      break;
    }
  }
  NET_PROCESS_REGISTE_IN(resp_login,NetModel::STATE_IN::RESP_LOGIN,netProcessRespLogin);
}

#endif
