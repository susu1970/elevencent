#ifndef NET_PROCESS_RESP_SIGNUP_HPP_
#define  NET_PROCESS_RESP_SIGNUP_HPP_

#include"net_process.h"
#include"app.h"
#include"event.h"
namespace qt_elevencent{
  static void netProcessRespSignup(void*arg){
    using namespace std;
    NetThread*thr=(NetThread*)arg;
    thr->m_retIn=NetModel::RETCODE::OK;
    if(thr->m_inList.empty())
      thr->m_inList.push_back(make_pair(NetModel::STATE_IN::RESP_SIGN_UP,new NetModel::StateCtx(new TcpProtocol::RespSignup,sizeof(TcpProtocol::RespSignup),[](NetModel::StateCtx*ctx){
	TcpProtocol::RespSignup*data=(TcpProtocol::RespSignup*)ctx->m_data;
	delete data;
      })));
    auto iter=thr->m_inList.begin();
    auto in=*iter;
    auto ctx=get<NetModel::StateCtx*>(in);
    TcpProtocol::RespSignup*data=(TcpProtocol::RespSignup*)ctx->m_data;
    int left=ctx->m_dataLen-ctx->m_dealLen;
    int n=thr->read((char*)data+ctx->m_dealLen,left);
    left-=n;
    ctx->m_dealLen+=n;
    if(left>0)
      return;
    data->ntoh();
    QString str="";
    if(data->isOk()||data->isWarn())
      if(data->code&TcpProtocol::RETCODE::WARN_RESOURCE_EXIST)
	str=QString("sign up err, username already exist");
      else
	str=QString("sign up success ");
    else if(data->isErr()){
      if(data->code&TcpProtocol::RETCODE::ERR_RLIMIT_FREQ)
	str="sign up too frequently, try again later";
      else
	str="sign up err";
    }
    thr->m_retIn|=NetModel::RETCODE::AGAIN_IN;
    thr->m_stateIn=NetModel::STATE_IN::START;
    App::getInstance()->postEvent(EVENT_TYPE_TOAST_SHOW,new ToastShowArg(str));
    delete ctx;
    thr->m_inList.erase(iter);
  }
  NET_PROCESS_REGISTE_IN(resp_sign_up,NetModel::STATE_IN::RESP_SIGN_UP,netProcessRespSignup);
}

#endif
