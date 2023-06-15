#ifndef TCP_PROCESS_USER_MSG_HPP_
#define TCP_PROCESS_USER_MSG_HPP_

#include"process.h"
#include"global.h"
#include"resource_global.h"
#include"resource.h"
#include"rsa.h"
#include<string>

namespace elevencent{
  class TcpProcessUserSendMsgCtx{
  public:
    TcpProtocol::UserSendMsg usend;
    enum STATE{
      USEND,
      BUF,
      OUT_NODE,
    };
    int offIn=0;
    STATE state=USEND;
    char*buf=0;
    TcpProcessContext::List::Node*outNode=0;        
    ~TcpProcessUserSendMsgCtx(){
      if(buf)
	delete[]buf;
      if(outNode)
	delete outNode;
    }
  };
  void*handleInProcessUserSendMsg(void*arg){
    using namespace std;
    using namespace ResourceLimit;
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    ctx->retIn=TcpProcessContext::RETCODE::OK;
    TcpProcessUserSendMsgCtx*localCtx=(TcpProcessUserSendMsgCtx*)ctx->ctxIn[TcpProcessContext::STATE_IN::USER_SEND_MSG];
    if(!localCtx){
      ctx->ctxIn[TcpProcessContext::STATE_IN::USER_SEND_MSG]=localCtx=new TcpProcessUserSendMsgCtx;
      ctx->registeOnDestroyFunc(TcpProcessContext::STATE_IN::USER_SEND_MSG,[](void*arg){
	TcpProcessContext*ctx=(TcpProcessContext*)arg;
	TcpProcessUserSendMsgCtx*localCtx=(TcpProcessUserSendMsgCtx*)(ctx->ctxIn[TcpProcessContext::STATE_IN::USER_SEND_MSG]);
	if(localCtx)
	  delete localCtx;
	ctx->ctxIn.erase(TcpProcessContext::STATE_IN::USER_SEND_MSG);
      });
    }
  state_again:
    switch(localCtx->state){
    case TcpProcessUserSendMsgCtx::STATE::USEND:{
      int left=sizeof(localCtx->usend)-localCtx->offIn;
      int err;
      int n=conn->read((char*)&localCtx->usend+localCtx->offIn,left,&err);
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
      localCtx->usend.ntoh();
      if(localCtx->usend.cryptlen>g_privkey.size()||localCtx->usend.nameLen>g_sizeLimit["name_resource_name"]||localCtx->usend.msgLen>g_sizeLimit["post_content_resource_post_content"]){
	DEBUG_PRETTY_MSG("user send msg error: length too long");
	ctx->retIn|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
	return arg;
      }
      localCtx->buf=new char[localCtx->usend.cryptlen+1];
      localCtx->buf[localCtx->usend.cryptlen]=0;
      localCtx->state=TcpProcessUserSendMsgCtx::STATE::BUF;
      localCtx->offIn=0;
      goto state_again;
    }
      break;
    case TcpProcessUserSendMsgCtx::STATE::BUF:{
      int left=localCtx->usend.cryptlen-localCtx->offIn;
      int err;
      int n=conn->read((char*)localCtx->buf+localCtx->offIn,left,&err);
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
      size_t len=RSA::decryptWithRand16(localCtx->buf,g_keypriv,localCtx->buf,localCtx->usend.cryptlen);
      if(len>localCtx->usend.cryptlen||len<sizeof(TcpProtocol::RespLogin::Token)){
	DEBUG_PRETTY_MSG("user send msg format error");
	ctx->retIn|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
	return arg;
      }
      TcpProtocol::RespLogin::Token*token=(TcpProtocol::RespLogin::Token*)localCtx->buf;
      char*name=(char*)(token+1);
      char*msg=name+localCtx->usend.nameLen+1;
      if(msg+localCtx->usend.msgLen!=localCtx->buf+len){
	DEBUG_PRETTY_MSG("user send msg format error");
	ctx->retIn|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
	return arg;	
      }
      msg[localCtx->usend.msgLen]=0;
      token->ntoh();      
      TcpProtocol::RespUserSendMsg*outresp=new TcpProtocol::RespUserSendMsg;
      do{
	if(ctx->inFreq.testAndUpdate(ResourceLimit::Frequency::DIMENSION_USER_SEND_MSG)){
	  outresp->code=TcpProtocol::RETCODE::ERR_RLIMIT_FREQ;
	  ctx->retIn|=TcpProcessContext::RETCODE::RLIMIT_FREQ;
	  break;
	}		
	if((*token)!=ctx->loginToken){
	  outresp->code=TcpProtocol::RETCODE::ERR_LOGIN_TOKEN;
	  break;
	}
	Resource::ScopeMutex rsm(g_resource);
	if(!g_resource->isSetResourceIdBitMap(token->uId)){
	  outresp->code=TcpProtocol::RETCODE::ERR_LOGIN_TOKEN;
	  break;
	}
	resource_id_t nId,nuId,pId;
	Resource::ERRNO err;
	if(g_resource->isErr(err=g_resource->selectNameResource(name,&nId))||g_resource->isErr(err=g_resource->selectUserResourceByNameId(&nuId,nId))){
	  outresp->code|=TcpProtocol::RETCODE::WARN_RESOURCE_EXIST;
	  break;
	}
	if(token->uId!=nuId){
	  if(g_resource->isErr(err=g_resource->insertPostContentResource(&pId,msg,localCtx->usend.msgLen+1))||g_resource->isErr(err=g_resource->userRefResource(token->uId,pId,RESOURCE_MASK_USER_POST_SENDER))||g_resource->isErr(err=g_resource->userRefResource(nuId,pId,RESOURCE_MASK_USER_POST_RECVER))){
	    outresp->code|=TcpProtocol::RETCODE::ERR_RESOURCE;
	    break;
	  }
	}else{
	  if(g_resource->isErr(err=g_resource->insertPostContentResource(&pId,msg,localCtx->usend.msgLen+1))||g_resource->isErr(err=g_resource->userRefResource(token->uId,pId,RESOURCE_MASK_USER_POST_SENDER|RESOURCE_MASK_USER_POST_RECVER))){
	    outresp->code|=TcpProtocol::RETCODE::ERR_RESOURCE;
	    break;
	  }
	}
      }while(0);
      outresp->hton();
      TcpProcessContext::DefaultOutCtx*outctx=new TcpProcessContext::DefaultOutCtx(outresp,sizeof(*outresp),[](TcpProcessContext::DefaultOutCtx*ctx){
	delete(TcpProtocol::RespUserSendMsg*)ctx->m_data;
      });
      localCtx->outNode=new TcpProcessContext::List::Node(TcpProcessContext::STATE_OUT::SEND_FIFO_GROUP,outctx,[](TcpProcessContext::List::Node*node){
	delete (TcpProcessContext::DefaultOutCtx*)node->ctx;
      });
      localCtx->state=TcpProcessUserSendMsgCtx::STATE::OUT_NODE;
      goto state_again;
    }
      break;
    case TcpProcessUserSendMsgCtx::STATE::OUT_NODE:{
      ctx->retIn|=TcpProcessContext::RETCODE::OUT_AGAIN;      
      if(ctx->outList.push_back(localCtx->outNode)){
	ctx->stateIn=TcpProcessContext::STATE_IN::START;
	ctx->retIn|=TcpProcessContext::RETCODE::NEXT;
	localCtx->outNode=0;
	ctx->ctxIn.erase(TcpProcessContext::STATE_IN::USER_SEND_MSG);
	delete localCtx;
	return arg;
      }
      ctx->retIn|=TcpProcessContext::RETCODE::IN_INSERT_EV;
      return arg;
    }
      break;
    default:
      break;
    }
    return arg;
  }
  TCP_PROCESS_REGISTE_IN(user_send_msg,TcpProcessContext::STATE_IN::USER_SEND_MSG,handleInProcessUserSendMsg);
  class TcpProcessReqUserMsgCtx{
  public:
    TcpProtocol::ReqUserMsg req;
    enum STATE{
      REQ,
      BUF,
      OUT_NODE,
      OUT_FRAG,
    };
    STATE state=REQ;
    char*buf=0;
    int offIn=0;
    TcpProcessContext::List::Node*outNode=0;
    bool validReq=false;
    resource_id_t nuid;
    std::string name="";
    TcpProcessContext*tcpCtx;
    sql::ResultSet*res=0;
    enum FRAG_STATE{
      RES_NEXT,
      FRAG_RES,
      FRAG_BLOB,
    };
    FRAG_STATE fragState=RES_NEXT;
    TcpProtocol::RspUserMsgFragPacket fragPacket;
    TcpProtocol::RspUserMsgFragPacket::Result fragRes;
    int offBlob=0;
    std::istream*fragIs=0;
    ~TcpProcessReqUserMsgCtx(){
      if(buf)
	delete[]buf;
      if(outNode)
	delete outNode;
      if(res)
	delete res;
    }
  };
  void*handleInProcessReqUserMsg(void*arg){
    using namespace std;
    using namespace ResourceLimit;
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    ctx->retIn=TcpProcessContext::RETCODE::OK;
    TcpProcessReqUserMsgCtx*localCtx=(TcpProcessReqUserMsgCtx*)ctx->ctxIn[TcpProcessContext::STATE_IN::USER_REQ_MSG];
    if(!localCtx){
      ctx->ctxIn[TcpProcessContext::STATE_IN::USER_REQ_MSG]=localCtx=new TcpProcessReqUserMsgCtx;
      ctx->registeOnDestroyFunc(TcpProcessContext::STATE_IN::USER_REQ_MSG,[](void*arg){
	TcpProcessContext*ctx=(TcpProcessContext*)arg;
	TcpProcessReqUserMsgCtx*localCtx=(TcpProcessReqUserMsgCtx*)ctx->ctxIn[TcpProcessContext::STATE_IN::USER_REQ_MSG];
	if(localCtx)
	  delete localCtx;
	ctx->ctxIn.erase(TcpProcessContext::STATE_IN::USER_REQ_MSG);
      });
    }
  state_again:
    switch(localCtx->state){
    case TcpProcessReqUserMsgCtx::STATE::REQ:{
      int left=sizeof(localCtx->req)-localCtx->offIn;
      int err;
      int n=conn->read((char*)&localCtx->req+localCtx->offIn,left,&err);
      left-=n;
      localCtx->offIn+=n;
      if(err&Connection::ERRNO_RLIMIT_QOS)
	ctx->retIn|=TcpProcessContext::RETCODE::RLIMIT_QOS;	
      if(err&Connection::ERRNO_CLOSE){
	ctx->retIn|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
	return arg;
      }
      if(left)
	return arg;
      localCtx->req.ntoh();
      if(localCtx->req.crylen>g_privkey.size()){
	ctx->retIn|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
	return arg;
      }
      localCtx->buf=new char[localCtx->req.crylen+1];
      localCtx->buf[localCtx->req.crylen]=0;
      localCtx->state=TcpProcessReqUserMsgCtx::STATE::BUF;
      localCtx->offIn=0;
      goto state_again;
    }
      break;
    case TcpProcessReqUserMsgCtx::STATE::BUF:{
      int left=localCtx->req.crylen-localCtx->offIn;
      int err;
      int n=conn->read((char*)localCtx->buf+localCtx->offIn,left,&err);
      left-=n;
      localCtx->offIn+=n;
      if(err&Connection::ERRNO_RLIMIT_QOS)
	ctx->retIn|=TcpProcessContext::RETCODE::RLIMIT_QOS;	
      if(err&Connection::ERRNO_CLOSE){
	ctx->retIn|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
	return arg;
      }
      if(left)
	return arg;
      size_t len=RSA::decryptWithRand16(localCtx->buf,g_keypriv,localCtx->buf,localCtx->req.crylen);
      if(len>localCtx->req.crylen||len<sizeof(TcpProtocol::ReqUserMsg::CryContent)){
	DEBUG_PRETTY_MSG("user req msg format error");
	ctx->retIn|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
	return arg;	
      }
      TcpProtocol::ReqUserMsg::CryContent*cryContent=(TcpProtocol::ReqUserMsg::CryContent*)localCtx->buf;
      cryContent->ntoh();
      if(cryContent->nameLen>g_sizeLimit["name_resource_name"]||len!=cryContent->nameLen+1+sizeof(TcpProtocol::ReqUserMsg::CryContent)||((char*)(cryContent+1))[cryContent->nameLen]){
	DEBUG_PRETTY_MSG("user req msg name error");
	ctx->retIn|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
	return arg;	
      }
      localCtx->name=(char*)(cryContent+1);
      //      DEBUG_PRETTY_MSG("name: "<<localCtx->name);
      TcpProtocol::RespUserReqMsg*outresp=new TcpProtocol::RespUserReqMsg;	  
      do{
	if(ctx->inFreq.testAndUpdate(ResourceLimit::Frequency::DIMENSION_USER_REQ_MSG)){
	  outresp->code=TcpProtocol::RETCODE::ERR_RLIMIT_FREQ;
	  ctx->retIn|=TcpProcessContext::RETCODE::RLIMIT_FREQ;
	  break;
	}			
	if(cryContent->token!=ctx->loginToken){
	  outresp->code|=TcpProtocol::RETCODE::ERR_LOGIN_TOKEN;
	  break;
	}
	Resource::ScopeMutex rsm(g_resource);
	resource_id_t nId;
	Resource::ERRNO err;
	if(g_resource->isErr(err=g_resource->selectNameResource(localCtx->name,&nId))||g_resource->isErr(err=g_resource->selectUserResourceByNameId(&localCtx->nuid,nId))||g_resource->isErr(err=g_resource->selectPostContentResource(cryContent->token.uId,localCtx->nuid,cryContent->from,cryContent->to,&localCtx->res))){
	  outresp->code|=TcpProtocol::RETCODE::WARN_RESOURCE_EXIST;
	  break;
	}
	localCtx->validReq=true;
      }while(0);
      outresp->hton();
      TcpProcessContext::DefaultOutCtx*outctx=new TcpProcessContext::DefaultOutCtx(outresp,sizeof(TcpProtocol::RespUserReqMsg),[](TcpProcessContext::DefaultOutCtx*ctx){
	delete(TcpProtocol::RespUserReqMsg*)ctx->m_data;
      });
      localCtx->outNode=new TcpProcessContext::List::Node(TcpProcessContext::STATE_OUT::SEND_FIFO_GROUP,outctx,[](TcpProcessContext::List::Node*node){
	delete (TcpProcessContext::DefaultOutCtx*)node->ctx;
      });
      localCtx->state=TcpProcessReqUserMsgCtx::STATE::OUT_NODE;
      goto state_again;
    }
      break;
    case TcpProcessReqUserMsgCtx::STATE::OUT_NODE:{
      ctx->retIn|=TcpProcessContext::RETCODE::OUT_AGAIN;
      if(ctx->outList.push_back(localCtx->outNode)){
	localCtx->outNode=0;
	if(!localCtx->validReq){
	  delete localCtx;
	  ctx->ctxIn.erase(TcpProcessContext::STATE_IN::USER_REQ_MSG);
	  ctx->retIn|=TcpProcessContext::RETCODE::NEXT;
	  ctx->stateIn=TcpProcessContext::STATE_IN::START;		  
	  return arg;
	}
	localCtx->tcpCtx=ctx;
	TcpProcessContext::OutFragCtx*fragCtx=new TcpProcessContext::OutFragCtx([](TcpProcessContext::OutFragCtx*fragCtx,void*buf,size_t count)->ssize_t{//read
	  TcpProcessReqUserMsgCtx*localCtx=(TcpProcessReqUserMsgCtx*)fragCtx->priv;
	  if(!localCtx->res)
	    return 0;
	  if(!buf){
	    return g_sizeLimit["tcp_process_frag_packet_size"];
	  }
	  TcpProtocol::RspUserMsgFragPacket*fragPacket=(TcpProtocol::RspUserMsgFragPacket*)buf;
	  switch(localCtx->fragState){
	  case TcpProcessReqUserMsgCtx::FRAG_STATE::RES_NEXT:{
	    if(!localCtx->res->next())
	      return 0;
	    TcpProtocol::RspUserMsgFragPacket::Result*fragRes=(TcpProtocol::RspUserMsgFragPacket::Result*)(fragPacket+1);
	    char*name=(char*)(fragRes+1);	    
	    localCtx->fragRes.umask=localCtx->res->getUInt64("user_resource_resource_mask");
	    localCtx->fragRes.pmask=localCtx->res->getUInt64("post_content_resource_resource_mask");
	    localCtx->fragRes.update_time=localCtx->res->getUInt64("user_resource_update_time");
	    localCtx->fragRes.name_len=localCtx->name.size();
	    localCtx->fragIs=localCtx->res->getBlob("post_content_resource_post_content");
	    localCtx->fragIs->seekg(0,std::ios::end);
	    localCtx->fragRes.post_content_len=localCtx->fragIs->tellg();
	    localCtx->fragIs->seekg(0,std::ios::beg);
	    localCtx->fragPacket.pack_id=TcpProtocol::FragPacket::getFreePackId();
	    localCtx->fragPacket.type=TcpProtocol::Head::TYPE::RESP_USER_REQ_MSG_FRAG;
	    localCtx->fragPacket.seq=0;
	    strncpy(name,localCtx->name.c_str(),localCtx->name.size());
	    name[localCtx->name.size()]=0;
	    memcpy(fragPacket,&localCtx->fragPacket,sizeof(localCtx->fragPacket));
	    memcpy(fragRes,&localCtx->fragRes,sizeof(localCtx->fragRes));
	    fragRes->hton();
	    string crypt=RSA::cryptWithRand16(fragRes,name+localCtx->name.size()+1-(char*)fragRes,localCtx->tcpCtx->pubkey);
	    if(crypt.empty()){
	      DEBUG_PRETTY_MSG("error : cyprt.empty!");
	      return 0;
	    }
	    memcpy(fragRes,crypt.c_str(),crypt.size());
	    //	    DEBUG_PRETTY_MSG("name: "<<name<<"pack_id: "<<fragPacket->pack_id<<", seq: "<<fragPacket->seq);
	    fragPacket->len=crypt.size();
	    fragPacket->hton();	    
	    localCtx->fragState=TcpProcessReqUserMsgCtx::FRAG_STATE::FRAG_BLOB;
	    return sizeof(TcpProtocol::RspUserMsgFragPacket)+crypt.size();
	  }
	  case TcpProcessReqUserMsgCtx::FRAG_STATE::FRAG_BLOB:{
	    ++localCtx->fragPacket.seq;
	    memcpy(fragPacket,&localCtx->fragPacket,sizeof(localCtx->fragPacket));
	    //	    DEBUG_PRETTY_MSG("pack_id: "<<fragPacket->pack_id<<", seq: "<<fragPacket->seq);	    
	    int left=MIN(localCtx->fragRes.post_content_len-localCtx->offBlob,g_sizeLimit["tcp_process_user_req_msg_blob_fetch_size"]);
	    if(left<=0)
	      return 0;
	    char*blob=(char*)(fragPacket+1);
	    localCtx->fragIs->read(blob,left);
	    //	    DEBUG_PRETTY_MSG("blob: "<<blob);	    
	    string crypt=RSA::cryptWithRand16(blob,left,localCtx->tcpCtx->pubkey);
	    localCtx->offBlob+=left;
	    if(localCtx->offBlob>=localCtx->fragRes.post_content_len){
	      localCtx->offBlob=0;
	      localCtx->fragState=TcpProcessReqUserMsgCtx::FRAG_STATE::RES_NEXT;
	    }	    
	    if(crypt.empty()){
	      DEBUG_PRETTY_MSG("error, crypt.empty!");
	      return 0;
	    }
	    memcpy(blob,crypt.c_str(),crypt.size());
	    fragPacket->len=crypt.size();
	    fragPacket->hton();
	    return sizeof(TcpProtocol::RspUserMsgFragPacket)+crypt.size();
	  }
	    break;
	  default:
	    break;
	  }
	  return 0;
	},[](TcpProcessContext::OutFragCtx*fragCtx){//on destroy
	  if(fragCtx->priv)
	    delete(TcpProcessReqUserMsgCtx*)fragCtx->priv;
	});
	localCtx->outNode=new TcpProcessContext::List::Node(TcpProcessContext::STATE_OUT::FRAG,fragCtx,[](TcpProcessContext::List::Node*node){
	  delete(TcpProcessContext::OutFragCtx*)node->ctx;
	});	
	localCtx->state=TcpProcessReqUserMsgCtx::STATE::OUT_FRAG;
	goto state_again;
      }
      ctx->retIn|=TcpProcessContext::RETCODE::IN_INSERT_EV;
      return arg;
    }
      break;
    case TcpProcessReqUserMsgCtx::STATE::OUT_FRAG:{
      ctx->retIn|=TcpProcessContext::RETCODE::OUT_AGAIN;      
      if(ctx->outFragList.push_back(localCtx->outNode)){
	TcpProcessContext::OutFragCtx*fragCtx=(TcpProcessContext::OutFragCtx*)localCtx->outNode->ctx;
	fragCtx->priv=localCtx;
	localCtx->outNode=0;
	ctx->ctxIn.erase(TcpProcessContext::STATE_IN::USER_REQ_MSG);
	ctx->retIn|=TcpProcessContext::RETCODE::NEXT;
	ctx->stateIn=TcpProcessContext::STATE_IN::START;
	return arg;
      }
      ctx->retIn|=TcpProcessContext::RETCODE::IN_INSERT_EV;
      return arg;
    }
      break;
    default:
      break;
    }
    return arg;
  }
  TCP_PROCESS_REGISTE_IN(user_req_msg,TcpProcessContext::STATE_IN::USER_REQ_MSG,handleInProcessReqUserMsg);
  
  class TcpProcessReqMsgUsersCtx{
  public:
    TcpProtocol::ReqMsgUsers req;
    enum STATE{
      REQ,
      BUF,
      OUT_NODE,
      OUT_FRAG,
    };
    STATE state=REQ;
    char*buf=0;
    int offIn=0;
    TcpProcessContext::List::Node*outNode=0;
    TcpProcessContext*tcpCtx=0;
    sql::ResultSet*res=0;
    ~TcpProcessReqMsgUsersCtx(){
      if(buf)
	delete[]buf;
      if(outNode)
	delete outNode;
      if(res)
	delete res;
    }
  };
  void*handleInProcessReqMsgUsers(void*arg){
    using namespace std;
    using namespace ResourceLimit;
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    ctx->retIn=TcpProcessContext::RETCODE::OK;
    TcpProcessReqMsgUsersCtx*localCtx=(TcpProcessReqMsgUsersCtx*)ctx->ctxIn[TcpProcessContext::STATE_IN::REQ_MSG_USERS];
    if(!localCtx){
      ctx->ctxIn[TcpProcessContext::STATE_IN::REQ_MSG_USERS]=localCtx=new TcpProcessReqMsgUsersCtx;
      ctx->registeOnDestroyFunc(TcpProcessContext::STATE_IN::REQ_MSG_USERS,[](void*arg){
	TcpProcessContext*ctx=(TcpProcessContext*)arg;
        TcpProcessReqMsgUsersCtx*localCtx=(TcpProcessReqMsgUsersCtx*)ctx->ctxIn[TcpProcessContext::STATE_IN::REQ_MSG_USERS];
	if(localCtx)
	  delete localCtx;
	ctx->ctxIn.erase(TcpProcessContext::STATE_IN::REQ_MSG_USERS);
      });
    }
  state_again:
    switch(localCtx->state){
    case TcpProcessReqMsgUsersCtx::STATE::REQ:{
      int left=sizeof(localCtx->req)-localCtx->offIn;
      int err;
      int n=conn->read((char*)&localCtx->req+localCtx->offIn,left,&err);
      left-=n;
      localCtx->offIn+=n;
      if(err&Connection::ERRNO_RLIMIT_QOS)
	ctx->retIn|=TcpProcessContext::RETCODE::RLIMIT_QOS;	
      if(err&Connection::ERRNO_CLOSE){
	ctx->retIn|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
	return arg;
      }
      if(left)
	return arg;
      localCtx->req.ntoh();
      if(localCtx->req.crylen>g_privkey.size()){
	ctx->retIn|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
	return arg;
      }
      localCtx->buf=new char[localCtx->req.crylen+1];
      localCtx->buf[localCtx->req.crylen]=0;
      localCtx->state=TcpProcessReqMsgUsersCtx::STATE::BUF;
      localCtx->offIn=0;
      goto state_again;
    }
      break;
    case TcpProcessReqMsgUsersCtx::STATE::BUF:{
      int left=localCtx->req.crylen-localCtx->offIn;
      int err;
      int n=conn->read((char*)localCtx->buf+localCtx->offIn,left,&err);
      left-=n;
      localCtx->offIn+=n;
      if(err&Connection::ERRNO_RLIMIT_QOS)
	ctx->retIn|=TcpProcessContext::RETCODE::RLIMIT_QOS;	
      if(err&Connection::ERRNO_CLOSE){
	ctx->retIn|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
	return arg;
      }
      if(left)
	return arg;
      size_t len=RSA::decryptWithRand16(localCtx->buf,g_keypriv,localCtx->buf,localCtx->req.crylen);
      if(len!=sizeof(TcpProtocol::RespLogin::Token)){
	DEBUG_PRETTY_MSG("req msg users format error");
	ctx->retIn|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
	return arg;	
      }
      TcpProtocol::RespLogin::Token*token=(TcpProtocol::RespLogin::Token*)localCtx->buf;
      token->ntoh();
      TcpProtocol::RespReqMsgUsers*outresp=new TcpProtocol::RespReqMsgUsers;      
      do{
	if(ctx->inFreq.testAndUpdate(ResourceLimit::Frequency::DIMENSION_REQ_MSG_USERS)){
	  outresp->code=TcpProtocol::RETCODE::ERR_RLIMIT_FREQ;
	  ctx->retIn|=TcpProcessContext::RETCODE::RLIMIT_FREQ;
	  break;
	}			
	if(*token!=ctx->loginToken){
	  outresp->code|=TcpProtocol::RETCODE::ERR_LOGIN_TOKEN;
	  break;
	}
	Resource::ScopeMutex rsm(g_resource);
	Resource::ERRNO err;
	if(g_resource->isErr(err=g_resource->selectMsgUsers(token->uId,&localCtx->res))){
	  outresp->code|=TcpProtocol::RETCODE::ERR_RESOURCE;
	  break;
	}
      }while(0);
      outresp->hton();
      TcpProcessContext::DefaultOutCtx*outctx=new TcpProcessContext::DefaultOutCtx(outresp,sizeof(TcpProtocol::RespReqMsgUsers),[](TcpProcessContext::DefaultOutCtx*ctx){
	delete(TcpProtocol::RespReqMsgUsers*)ctx->m_data;
      });
      localCtx->outNode=new TcpProcessContext::List::Node(TcpProcessContext::STATE_OUT::SEND_FIFO_GROUP,outctx,[](TcpProcessContext::List::Node*node){
	delete (TcpProcessContext::DefaultOutCtx*)node->ctx;
      });
      localCtx->state=TcpProcessReqMsgUsersCtx::STATE::OUT_NODE;
      goto state_again;
    }
      break;
    case TcpProcessReqMsgUsersCtx::STATE::OUT_NODE:{
      ctx->retIn|=TcpProcessContext::RETCODE::OUT_AGAIN;
      if(ctx->outList.push_back(localCtx->outNode)){
	localCtx->outNode=0;
	if(!localCtx->res){
	  delete localCtx;
	  ctx->ctxIn.erase(TcpProcessContext::STATE_IN::REQ_MSG_USERS);
	  ctx->retIn|=TcpProcessContext::RETCODE::NEXT;
	  ctx->stateIn=TcpProcessContext::STATE_IN::START;		  
	  return arg;
	}
	localCtx->tcpCtx=ctx;
	TcpProcessContext::OutFragCtx*fragCtx=new TcpProcessContext::OutFragCtx([](TcpProcessContext::OutFragCtx*fragCtx,void*buf,size_t count)->ssize_t{//read
	  TcpProcessReqMsgUsersCtx*localCtx=(TcpProcessReqMsgUsersCtx*)fragCtx->priv;
	  if(!localCtx->res)
	    return 0;
	  if(!buf){
	    return g_sizeLimit["tcp_process_frag_packet_size"];
	  }
	  if(!localCtx->res->next())
	    return 0;
	  TcpProtocol::RspMsgUsersFragPacket*fragPacket=(TcpProtocol::RspMsgUsersFragPacket*)buf;
	  sql::SQLString name=localCtx->res->getString("name");
	  //	  DEBUG_PRETTY_MSG("req msg users sqlname: "<<name.c_str());
	  fragPacket->type=TcpProtocol::Head::TYPE::RESP_MSG_USERS_FRAG;
	  fragPacket->seq=0;
	  fragPacket->pack_id=TcpProtocol::FragPacket::getFreePackId();
	  uint16_t*nameLen=(uint16_t*)(fragPacket+1);
	  *nameLen=name.size();
	  *nameLen=elevencent::hton<uint16_t>(*nameLen);
	  char*bufName=(char*)(nameLen+1);
	  memcpy(bufName,name.c_str(),name.size());
	  bufName[name.size()]=0;
	  string crypt=RSA::cryptWithRand16(nameLen,sizeof(*nameLen)+name.size()+1,localCtx->tcpCtx->pubkey);
	  if(crypt.empty()){
	    DEBUG_PRETTY_MSG("crypt error");
	    return 0;
	  }
	  fragPacket->len=crypt.size();
	  memcpy(fragPacket+1,crypt.c_str(),crypt.size());
	  fragPacket->hton();
	  return sizeof(TcpProtocol::RspMsgUsersFragPacket)+crypt.size();
	},[](TcpProcessContext::OutFragCtx*fragCtx){//on destroy
	  if(fragCtx->priv)
	    delete(TcpProcessReqMsgUsersCtx*)fragCtx->priv;
	});
	localCtx->outNode=new TcpProcessContext::List::Node(TcpProcessContext::STATE_OUT::FRAG,fragCtx,[](TcpProcessContext::List::Node*node){
	  delete(TcpProcessContext::OutFragCtx*)node->ctx;
	});	
	localCtx->state=TcpProcessReqMsgUsersCtx::STATE::OUT_FRAG;
	goto state_again;
      }
      ctx->retIn|=TcpProcessContext::RETCODE::IN_INSERT_EV;
      return arg;
    }
      break;
    case TcpProcessReqMsgUsersCtx::STATE::OUT_FRAG:{
      ctx->retIn|=TcpProcessContext::RETCODE::OUT_AGAIN;      
      if(ctx->outFragList.push_back(localCtx->outNode)){
	TcpProcessContext::OutFragCtx*fragCtx=(TcpProcessContext::OutFragCtx*)localCtx->outNode->ctx;
	fragCtx->priv=localCtx;
	localCtx->outNode=0;
	ctx->ctxIn.erase(TcpProcessContext::STATE_IN::REQ_MSG_USERS);
	ctx->retIn|=TcpProcessContext::RETCODE::NEXT;
	ctx->stateIn=TcpProcessContext::STATE_IN::START;
	return arg;
      }
      ctx->retIn|=TcpProcessContext::RETCODE::IN_INSERT_EV;
      return arg;
      
    }
      break;
    default:
      break;
    }
    return arg;
  }
  TCP_PROCESS_REGISTE_IN(req_msg_users,TcpProcessContext::STATE_IN::REQ_MSG_USERS,handleInProcessReqMsgUsers);
  
  
}

#endif
