#ifndef NET_PROCESS_HEAD_HPP_
#define  NET_PROCESS_HEAD_HPP_

#include"net_process.h"
#include"net_model.h"
#include"cmd_model.h"

namespace qt_elevencent{
  static void netProcessHandleInHead(void*arg){
    using namespace std;
    NetThread*thr=(NetThread*)arg;
    thr->m_retIn=NetModel::RETCODE::OK;
    if(thr->m_inList.empty())
      thr->m_inList.push_back(make_pair(NetModel::STATE_IN::HEAD,new NetModel::StateCtx(new TcpProtocol::Head,sizeof(TcpProtocol::Head),[](NetModel::StateCtx*ctx){
	delete (TcpProtocol::Head*)ctx->m_data;
      })));
    auto iter=thr->m_inList.begin();
    auto in=*iter;
    auto ctx=get<NetModel::StateCtx*>(in);
    int left=ctx->m_dataLen-ctx->m_dealLen;
    int n=thr->read((char*)ctx->m_data+ctx->m_dealLen,left);
    left-=n;
    ctx->m_dealLen+=n;
    if(left>0)
      return;
    TcpProtocol::Head*head=(TcpProtocol::Head*)ctx->m_data;
    head->n2h();
    QString str=QString::fromStdString(string("received head,type: "))+QString::number(head->type)+"\n";
    qDebug()<<str;
    switch(head->type){
    case TcpProtocol::Head::TYPE::RESP_PUBKEY:
      thr->m_stateIn=NetModel::STATE_IN::RESP_PUBKEY;
      thr->m_inList.pop_front();
      delete ctx;
      thr->m_retIn|=NetModel::RETCODE::AGAIN_IN;            
      break;
    case TcpProtocol::Head::TYPE::RESP_SIGN_UP:
      thr->m_stateIn=NetModel::STATE_IN::RESP_SIGN_UP;
      thr->m_inList.pop_front();
      delete ctx;
      thr->m_retIn|=NetModel::RETCODE::AGAIN_IN;            
      break;
    case TcpProtocol::Head::TYPE::RESP_LOGIN:
      thr->m_stateIn=NetModel::STATE_IN::RESP_LOGIN;
      thr->m_inList.pop_front();
      delete ctx;
      thr->m_retIn|=NetModel::RETCODE::AGAIN_IN;            
      break;
    case TcpProtocol::Head::TYPE::RESP_USER_SEND_MSG:
      thr->m_stateIn=NetModel::STATE_IN::RESP_USER_SEND_MSG;
      thr->m_inList.pop_front();
      delete ctx;
      thr->m_retIn|=NetModel::RETCODE::AGAIN_IN;            
      break;
    case TcpProtocol::Head::TYPE::RESP_USER_REQ_MSG:
      thr->m_stateIn=NetModel::STATE_IN::RESP_USER_REQ_MSG;
      thr->m_inList.pop_front();
      delete ctx;
      thr->m_retIn|=NetModel::RETCODE::AGAIN_IN;            
      break;
    case TcpProtocol::Head::TYPE::RESP_USER_REQ_MSG_FRAG:
      thr->m_stateIn=NetModel::STATE_IN::RESP_USER_REQ_MSG_FRAG;
      thr->m_inList.pop_front();
      delete ctx;
      thr->m_retIn|=NetModel::RETCODE::AGAIN_IN;            
      break;            
    case TcpProtocol::Head::TYPE::RESP_MSG_USERS_CODE:
      thr->m_stateIn=NetModel::STATE_IN::RESP_MSG_USERS_CODE;
      thr->m_inList.pop_front();
      delete ctx;
      thr->m_retIn|=NetModel::RETCODE::AGAIN_IN;            
      break;
    case TcpProtocol::Head::TYPE::RESP_MSG_USERS_FRAG:
      thr->m_stateIn=NetModel::STATE_IN::RESP_MSG_USERS_FRAG;
      thr->m_inList.pop_front();
      delete ctx;
      thr->m_retIn|=NetModel::RETCODE::AGAIN_IN;            
      break;            
      
    default:
      thr->m_inList.pop_front();
      delete ctx;
      thr->m_retIn|=NetModel::RETCODE::INVALID_PACKET;
      break;
    }
  }
  NET_PROCESS_REGISTE_IN(head,NetModel::STATE_IN::HEAD,netProcessHandleInHead);
}

#endif
