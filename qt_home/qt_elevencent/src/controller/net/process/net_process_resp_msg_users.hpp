#ifndef NET_PROCESS_RESP_MSG_USERS_HPP_
#define NET_PROCESS_RESP_MSG_USERS_HPP_

#include"net_process.h"
#include"app.h"
#include"event.h"

namespace qt_elevencent{
  static void netProcessRespMsgUsersCode(void*arg){
    using namespace std;
    NetThread*thr=(NetThread*)arg;
    thr->m_retIn=NetModel::RETCODE::OK;
    if(thr->m_inList.empty())
      thr->m_inList.push_back(make_pair(NetModel::STATE_IN::RESP_MSG_USERS_CODE,new NetModel::StateCtx(new TcpProtocol::RespMsgUsersCode,sizeof(TcpProtocol::RespMsgUsersCode),[](NetModel::StateCtx*ctx){
	delete(TcpProtocol::RespMsgUsersCode*)ctx->m_data;
      })));
    auto iter=thr->m_inList.begin();
    auto in=*iter;
    auto ctx=get<NetModel::StateCtx*>(in);
    TcpProtocol::RespMsgUsersCode*data=(TcpProtocol::RespMsgUsersCode*)ctx->m_data;
    int left=ctx->m_dataLen-ctx->m_dealLen;
    int n=thr->read((char*)data+ctx->m_dealLen,left);
    left-=n;
    ctx->m_dealLen+=n;
    if(left>0)
      return;
    data->ntoh();
    QString str="";
    if(data->isErr()){
      if(data->code&TcpProtocol::RETCODE::ERR_RLIMIT_FREQ)
	str="msg users code too frequently, try again later";
      else if(data->code&TcpProtocol::RETCODE::ERR_LOGIN_TOKEN)
	str="msg users code err, not login";
      else if(data->code&TcpProtocol::RETCODE::ERR_RESOURCE)
	str="msg users code err, resource err";
    }else{
      str="msg users code success";
    }
    qDebug()<<str;
    thr->m_retIn|=NetModel::RETCODE::AGAIN_IN;
    thr->m_stateIn=NetModel::STATE_IN::START;
    delete ctx;
    thr->m_inList.erase(iter);
  }
  NET_PROCESS_REGISTE_IN(resp_msg_users_code,NetModel::STATE_IN::RESP_MSG_USERS_CODE,netProcessRespMsgUsersCode);
  class TcpProcessRespMsgUsersFragCtx{
  public:
    enum STATE{
      FRAG_PACKET,
      BUF,
    };
    STATE state=FRAG_PACKET;
    TcpProtocol::RspMsgUsersFragPacket fragPacket;
    char*buf=0;
    ~TcpProcessRespMsgUsersFragCtx(){
      if(buf)
	delete[]buf;
    }
  };
  static void netProcessRespMsgUsersFrag(void*arg){
    using namespace std;
    NetThread*thr=(NetThread*)arg;
    thr->m_retIn=NetModel::RETCODE::OK;
    if(thr->m_inList.empty())
      thr->m_inList.push_back(make_pair(NetModel::STATE_IN::RESP_MSG_USERS_FRAG,new NetModel::StateCtx(new TcpProcessRespMsgUsersFragCtx,sizeof(TcpProcessRespMsgUsersFragCtx),[](NetModel::StateCtx*ctx){
	if(ctx->m_data)
	  delete(TcpProcessRespMsgUsersFragCtx*)ctx->m_data;
      })));
    auto iter=thr->m_inList.begin();
    auto in=*iter;
    auto ctx=get<NetModel::StateCtx*>(in);
    TcpProcessRespMsgUsersFragCtx*fragCtx=(TcpProcessRespMsgUsersFragCtx*)ctx->m_data;
  state_again:
    switch(fragCtx->state){
    case TcpProcessRespMsgUsersFragCtx::STATE::FRAG_PACKET:{
      int left=sizeof(fragCtx->fragPacket)-ctx->m_dealLen;
      int n=thr->read((char*)&fragCtx->fragPacket+ctx->m_dealLen,left);
      left-=n;
      ctx->m_dealLen+=n;
      if(left>0)
	return;
      fragCtx->fragPacket.ntoh();
      qDebug()<<"frag_packet, pack_id: "<<fragCtx->fragPacket.pack_id<<", seq: "<<fragCtx->fragPacket.seq<<", len: "<<fragCtx->fragPacket.len;
      if(fragCtx->fragPacket.len>1024*1024){
	qDebug()<<"fragCtx->fragPacket.len>1024*1024";
	thr->m_retIn|=NetModel::RETCODE::INVALID_PACKET;
	thr->m_stateIn=NetModel::STATE_IN::START;	
	return;	
      }
      fragCtx->buf=new char[fragCtx->fragPacket.len+1];
      fragCtx->buf[fragCtx->fragPacket.len]=0;
      ctx->m_dealLen=0;
      fragCtx->state=TcpProcessRespMsgUsersFragCtx::STATE::BUF;
      goto state_again;
    }
    case TcpProcessRespMsgUsersFragCtx::STATE::BUF:{
      int left=fragCtx->fragPacket.len-ctx->m_dealLen;
      int n=thr->read((char*)fragCtx->buf+ctx->m_dealLen,left);
      left-=n;
      ctx->m_dealLen+=n;
      if(left>0)
	return;
      size_t len=RSA::decryptWithRand16(fragCtx->buf,g_keypriv,fragCtx->buf,fragCtx->fragPacket.len);
      if(len<sizeof(uint16_t)){
	qDebug()<<"err, len("<<len<<")<sizeof(uint16_t)";
	thr->m_retIn|=NetModel::RETCODE::INVALID_PACKET;
	thr->m_stateIn=NetModel::STATE_IN::START;
	return;
      }
      uint16_t*name_len=(uint16_t*)fragCtx->buf;
      *name_len=qFromBigEndian<uint16_t>(*name_len);
      if(*name_len>4000||*name_len+1+sizeof(*name_len)!=len){
	qDebug()<<"err, *name_len>4000||*name_len+sizeof(*name_len)!=len";
	thr->m_retIn|=NetModel::RETCODE::INVALID_PACKET;
	thr->m_stateIn=NetModel::STATE_IN::START;
	return;
      }

      char*name=(char*)(name_len+1);
      qDebug()<<"name_len: "<<name_len<<", name: "<<name;
      if(name[*name_len]){
	qDebug()<<"err, name[*name_len]";
	thr->m_retIn|=NetModel::RETCODE::INVALID_PACKET;
	thr->m_stateIn=NetModel::STATE_IN::START;
	return;	
      }
      qDebug()<<"pack_id: "<<fragCtx->fragPacket.pack_id<<", seq: "<<fragCtx->fragPacket.seq<<", len: "<<fragCtx->fragPacket.len<<", name_len: "<<*name_len<<", name: "<<name;      
      delete ctx;
      thr->m_inList.erase(iter);
      thr->m_retIn|=NetModel::RETCODE::AGAIN_IN;
      thr->m_stateIn=NetModel::STATE_IN::START;      
    }
      break;
    default:
      break;
    }
  }  
  NET_PROCESS_REGISTE_IN(resp_msg_users_frag,NetModel::STATE_IN::RESP_MSG_USERS_FRAG,netProcessRespMsgUsersFrag);  
  
}

#endif
