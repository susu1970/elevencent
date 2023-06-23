#ifndef NET_PROCESS_RESP_USER_REQ_MSG_HPP_
#define NET_PROCESS_RESP_USER_REQ_MSG_HPP_

#include"net_process.h"
#include"app.h"
#include"event.h"
#include"msg_model.h"

namespace qt_elevencent{
  static void netProcessRespUserReqMsg(void*arg){
    using namespace std;
    NetThread*thr=(NetThread*)arg;
    thr->m_retIn=NetModel::RETCODE::OK;
    if(thr->m_inList.empty())
      thr->m_inList.push_back(make_pair(NetModel::STATE_IN::RESP_USER_REQ_MSG,new NetModel::StateCtx(new TcpProtocol::RespUserReqMsg,sizeof(TcpProtocol::RespUserReqMsg),[](NetModel::StateCtx*ctx){
	delete(TcpProtocol::RespUserReqMsg*)ctx->m_data;
      })));
    auto iter=thr->m_inList.begin();
    auto in=*iter;
    auto ctx=get<NetModel::StateCtx*>(in);
    TcpProtocol::RespUserReqMsg*data=(TcpProtocol::RespUserReqMsg*)ctx->m_data;
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
	str="user req msg too frequently, try again later";
      else if(data->code&TcpProtocol::RETCODE::ERR_LOGIN_TOKEN)
	str="user req msg err, not login";
      else
	str="user req msg err";
    }else if(data->isWarn()){
      if(data->code&TcpProtocol::RETCODE::WARN_RESOURCE_EXIST){
	str="user req msg err, user not exist";
      }
    }
    //    qDebug()<<str;
    thr->m_retIn|=NetModel::RETCODE::AGAIN_IN;
    thr->m_stateIn=NetModel::STATE_IN::START;
    //    App::getInstance()->postEvent(EVENT_TYPE_TOAST_SHOW,new ToastShowArg(str));
    delete ctx;
    thr->m_inList.erase(iter);
  }
  NET_PROCESS_REGISTE_IN(resp_user_req_msg,NetModel::STATE_IN::RESP_USER_REQ_MSG,netProcessRespUserReqMsg);
  class TcpProcessRespUserReqMsgPackIdCtx{
  public:
    char*name=0;
    char*resbuf=0;
    char*blobbuf=0;
    ~TcpProcessRespUserReqMsgPackIdCtx(){
      if(name)
	delete[]name;
      if(resbuf)
	delete[]resbuf;
      if(blobbuf)
	delete[]blobbuf;
    }
  };
  class TcpProcessRespUserReqMsgFragCtx{
  public:
    enum STATE{
      FRAG_PACKET_RESULT,
      RESULT,
      FRAG_PACKET_BLOB,
      BLOB,
    };
    STATE state=FRAG_PACKET_RESULT;
    TcpProtocol::RspUserMsgFragPacket fragPacket;
    TcpProtocol::RspUserMsgFragPacket::Result fragRes;
    char*resbuf=0;
    char*blobbuf=0;
    ~TcpProcessRespUserReqMsgFragCtx(){
      if(resbuf)
	delete[]resbuf;
      if(blobbuf)
	delete[]blobbuf;
    }
  };
  static void netProcessRespUserSendMsgFrag(void*arg){
    using namespace std;
    NetThread*thr=(NetThread*)arg;
    thr->m_retIn=NetModel::RETCODE::OK;
    if(thr->m_inList.empty())
      thr->m_inList.push_back(make_pair(NetModel::STATE_IN::RESP_USER_REQ_MSG_FRAG,new NetModel::StateCtx(new TcpProcessRespUserReqMsgFragCtx,sizeof(TcpProcessRespUserReqMsgFragCtx),[](NetModel::StateCtx*ctx){
	if(ctx->m_data)
	  delete(TcpProcessRespUserReqMsgFragCtx*)ctx->m_data;
      })));
    auto iter=thr->m_inList.begin();
    auto in=*iter;
    auto ctx=get<NetModel::StateCtx*>(in);
    TcpProcessRespUserReqMsgFragCtx*fragCtx=(TcpProcessRespUserReqMsgFragCtx*)ctx->m_data;
  state_again:
    switch(fragCtx->state){
    case TcpProcessRespUserReqMsgFragCtx::STATE::FRAG_PACKET_RESULT:{
      int left=sizeof(TcpProtocol::RspUserMsgFragPacket)-ctx->m_dealLen;
      int n=thr->read((char*)&fragCtx->fragPacket+ctx->m_dealLen,left);
      left-=n;
      ctx->m_dealLen+=n;
      if(left>0)
	return;
      fragCtx->fragPacket.ntoh();
      if(fragCtx->fragPacket.len>1024*1024){
	qDebug()<<"fragCtx->fragPacket.len>1024*1024";
	thr->m_retIn|=NetModel::RETCODE::INVALID_PACKET;
	thr->m_stateIn=NetModel::STATE_IN::START;	
	return;	
      }
      if(!fragCtx->fragPacket.seq){
	fragCtx->state=TcpProcessRespUserReqMsgFragCtx::STATE::RESULT;
	fragCtx->resbuf=new char[fragCtx->fragPacket.len+1];
	fragCtx->resbuf[fragCtx->fragPacket.len]=0;	
      }else{
	fragCtx->state=TcpProcessRespUserReqMsgFragCtx::STATE::BLOB;
	fragCtx->blobbuf=new char[fragCtx->fragPacket.len+1];
	fragCtx->blobbuf[fragCtx->fragPacket.len]=0;	
      }
      ctx->m_dealLen=0;
      goto state_again;
    }
    case TcpProcessRespUserReqMsgFragCtx::STATE::RESULT:{
      int left=fragCtx->fragPacket.len-ctx->m_dealLen;
      int n=thr->read((char*)fragCtx->resbuf+ctx->m_dealLen,left);
      left-=n;
      ctx->m_dealLen+=n;
      if(left>0)
	return;
      size_t len=RSA::decryptWithRand16(fragCtx->resbuf,g_keypriv,fragCtx->resbuf,fragCtx->fragPacket.len);
      if(len<sizeof(TcpProtocol::RspUserMsgFragPacket::Result)){
	qDebug()<<"len<sizeof(TcpProtocol::RspUserMsgFragPacket::Result)";
	thr->m_retIn|=NetModel::RETCODE::INVALID_PACKET;
	thr->m_stateIn=NetModel::STATE_IN::START;
	return;
      }
      TcpProtocol::RspUserMsgFragPacket::Result*res=(TcpProtocol::RspUserMsgFragPacket::Result*)fragCtx->resbuf;      
      res->ntoh();
      if(res->name_len>4000){
	thr->m_retIn|=NetModel::RETCODE::INVALID_PACKET;
	qDebug()<<"error, res->name_len>4000";
	return;
      }
      char*name=(char*)(res+1);
      if(len!=res->name_len+sizeof(TcpProtocol::RspUserMsgFragPacket::Result)+1||name[res->name_len]){
	qDebug()<<"error, len!=res->name_len+sizeof(TcpProtocol::RspUserMsgFragPacket::Result)||name[res->name_len]";
	thr->m_retIn|=NetModel::RETCODE::INVALID_PACKET;
	return;
      }
      if(thr->m_fragPacketCtx.find(fragCtx->fragPacket.pack_id)!=thr->m_fragPacketCtx.end()){
	auto fragPair=thr->m_fragPacketCtx[fragCtx->fragPacket.pack_id];
	fragPair.second(fragPair.first);
	thr->m_fragPacketCtx.erase(fragCtx->fragPacket.pack_id);
      }      
      if(thr->m_fragPacketCtx.size()<thr->ms_maxFragPacketSize){
	thr->m_fragPacketCtx[fragCtx->fragPacket.pack_id]=make_pair(fragCtx,[](void*ctx){
	  TcpProcessRespUserReqMsgFragCtx*fragCtx=(TcpProcessRespUserReqMsgFragCtx*)ctx;
	  if(fragCtx)
	    delete fragCtx;
	});
	ctx->m_data=0;
      }else{
	qDebug()<<"too many frag packet";
	thr->m_retIn|=NetModel::RETCODE::INVALID_PACKET;	
      }
      delete ctx;
      thr->m_inList.erase(iter);
      thr->m_retIn|=NetModel::RETCODE::AGAIN_IN;
      thr->m_stateIn=NetModel::STATE_IN::START;      
    }
      break;
    case TcpProcessRespUserReqMsgFragCtx::STATE::BLOB:{
      int left=fragCtx->fragPacket.len-ctx->m_dealLen;
      int n=thr->read((char*)fragCtx->blobbuf+ctx->m_dealLen,left);
      left-=n;
      ctx->m_dealLen+=n;
      if(left>0)
	return;
      //      qDebug()<<"blob, pack_id: "<<fragCtx->fragPacket.pack_id<<", seq: "<<fragCtx->fragPacket.seq<<", len: "<<fragCtx->fragPacket.len;            
      size_t len=RSA::decryptWithRand16(fragCtx->blobbuf,g_keypriv,fragCtx->blobbuf,fragCtx->fragPacket.len);
      if(thr->m_fragPacketCtx.find(fragCtx->fragPacket.pack_id)==thr->m_fragPacketCtx.end()){
	qDebug()<<"thr->m_fragPacketCtx.find(fragCtx->fragPacket.pack_id)==thr->m_fragPacketCtx.end()";
	thr->m_retIn|=NetModel::RETCODE::INVALID_PACKET;
	return;
      }
      char*blob=(char*)fragCtx->blobbuf;
      blob[len]=0;      
      auto fragPair=thr->m_fragPacketCtx[fragCtx->fragPacket.pack_id];
      TcpProcessRespUserReqMsgFragCtx*fragPairCtx=(TcpProcessRespUserReqMsgFragCtx*)fragPair.first;
      fragPairCtx->blobbuf=fragCtx->blobbuf;
      fragCtx->blobbuf=0;
      TcpProtocol::RspUserMsgFragPacket::Result*res=(TcpProtocol::RspUserMsgFragPacket::Result*)fragPairCtx->resbuf;      
      char*name=(char*)((TcpProtocol::RspUserMsgFragPacket::Result*)(fragPairCtx->resbuf)+1);
      App::getInstance()->postEvent(EVENT_TYPE_RESP_USER_REQ_MSG,new MsgModel::EventRespUserReqMsg(name,MsgModel::MsgUnit(res->umask,res->pmask,res->update_time,blob)));      
      //      qDebug()<<"name: "<<name<<", blob: "<<blob;
      fragPair.second(fragPair.first);
      thr->m_fragPacketCtx.erase(fragCtx->fragPacket.pack_id);
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
  NET_PROCESS_REGISTE_IN(resp_user_req_msg_frag,NetModel::STATE_IN::RESP_USER_REQ_MSG_FRAG,netProcessRespUserSendMsgFrag);  
  
}

#endif
