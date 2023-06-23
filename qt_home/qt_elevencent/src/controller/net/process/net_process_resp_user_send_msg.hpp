#ifndef NET_PROCESS_RESP_USER_SEND_MSG_HPP_
#define  NET_PROCESS_RESP_USER_SEND_MSG_HPP_

#include"net_process.h"
#include"app.h"
#include"event.h"
namespace qt_elevencent{
  static void netProcessRespUserSendMsg(void*arg){
    using namespace std;
    NetThread*thr=(NetThread*)arg;
    thr->m_retIn=NetModel::RETCODE::OK;
    if(thr->m_inList.empty())
      thr->m_inList.push_back(make_pair(NetModel::STATE_IN::RESP_USER_SEND_MSG,new NetModel::StateCtx(new TcpProtocol::RespUserSendMsg,sizeof(TcpProtocol::RespUserSendMsg),[](NetModel::StateCtx*ctx){
	delete(TcpProtocol::RespUserSendMsg*)ctx->m_data;
      })));
    auto iter=thr->m_inList.begin();
    auto in=*iter;
    auto ctx=get<NetModel::StateCtx*>(in);
    TcpProtocol::RespUserSendMsg*data=(TcpProtocol::RespUserSendMsg*)ctx->m_data;
    int left=ctx->m_dataLen-ctx->m_dealLen;
    int n=thr->read((char*)data+ctx->m_dealLen,left);
    left-=n;
    ctx->m_dealLen+=n;
    if(left>0)
      return;
    data->ntoh();
    QString str="";
    if(data->isOk()||data->isWarn()){
      if(data->code&TcpProtocol::RETCODE::WARN_RESOURCE_EXIST)
	str=QString("user send msg error, name not exist");
      else
	str=QString("user send msg success ");      
    }else if(data->isErr()){
      if(data->code&TcpProtocol::RETCODE::ERR_RLIMIT_FREQ)
	str="user send msg too frequently, try again later";
      else if(data->code&TcpProtocol::RETCODE::ERR_LOGIN_TOKEN)
	str="user send msg err, not login";
      else if(data->code&TcpProtocol::RETCODE::ERR_RESOURCE)
	str="user send msg err, resource error";	
      else
	str="user send msg err";
    }
    thr->m_retIn|=NetModel::RETCODE::AGAIN_IN;
    thr->m_stateIn=NetModel::STATE_IN::START;
    App::getInstance()->postEvent(EVENT_TYPE_TOAST_SHOW,new ToastShowArg(str,1000));
    delete ctx;
    thr->m_inList.erase(iter);
  }
  NET_PROCESS_REGISTE_IN(resp_user_send_msg,NetModel::STATE_IN::RESP_USER_SEND_MSG,netProcessRespUserSendMsg);
}

#endif
