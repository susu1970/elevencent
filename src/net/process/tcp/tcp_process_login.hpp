#ifndef TCP_PROCESS_LOGIN_HPP_
#define  TCP_PROCESS_LOGIN_HPP_

#include"process.h"
#include"global.h"
#include"resource_global.h"
#include"resource.h"
#include"rsa.h"

namespace elevencent{
  class TcpProcessLoginContext{
  public:
    int offIn=0;
    char*name=0;
    char*crypwd=0;
    enum STATE{
      LOGIN,
      NAME,
      PWD,
      OUT_NODE,
    };
    STATE state=LOGIN;
    TcpProtocol::Login login;
    TcpProcessContext::List::Node*outNode=0;    
    ~TcpProcessLoginContext(){
      if(name)
	delete[]name;
      if(crypwd)
	delete[]crypwd;
      if(outNode)
	delete outNode;
    }
  };
  void*handleInProcessLogin(void*arg){
    using namespace std;
    using namespace ResourceLimit;
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    ctx->retIn=TcpProcessContext::RETCODE::OK;    
    TcpProcessLoginContext*localCtx=(TcpProcessLoginContext*)ctx->ctxIn[TcpProcessContext::STATE_IN::LOGIN];
    if(!localCtx){
      ctx->ctxIn[TcpProcessContext::STATE_IN::LOGIN]=localCtx=new TcpProcessLoginContext;
      ctx->registeOnDestroyFunc(TcpProcessContext::STATE_IN::LOGIN,[](void*arg){
	TcpProcessContext*ctx=(TcpProcessContext*)arg;
	TcpProcessLoginContext*localCtx=(TcpProcessLoginContext*)ctx->ctxIn[TcpProcessContext::STATE_IN::LOGIN];
	if(localCtx)
	  delete localCtx;
	ctx->ctxIn.erase(TcpProcessContext::STATE_IN::LOGIN);
      });
    }
  state_again:
    switch(localCtx->state){
    case TcpProcessLoginContext::STATE::LOGIN:{
      int left=sizeof(TcpProtocol::Login)-localCtx->offIn;
      int err;
      int n=conn->read((char*)&localCtx->login+localCtx->offIn,left,&err);
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
      localCtx->login.ntoh();
      if(localCtx->login.nameLen>g_sizeLimit["name_resource_name"]||localCtx->login.crypwdLen>g_sizeLimit["passwd_resource_passwd"]){
	DEBUG_PRETTY_MSG("login string length too long");
	ctx->retIn|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
	return arg;
      }
      localCtx->name=new char[localCtx->login.nameLen+1];
      localCtx->name[localCtx->login.nameLen]=0;
      localCtx->crypwd=new char[localCtx->login.crypwdLen+1];
      localCtx->crypwd[localCtx->login.crypwdLen]=0;
      localCtx->state=TcpProcessLoginContext::STATE::NAME;
      localCtx->offIn=0;
      goto state_again;
    }
      break;
    case TcpProcessLoginContext::STATE::NAME:{
      int left=localCtx->login.nameLen-localCtx->offIn;
      int err;
      int n=conn->read((char*)localCtx->name+localCtx->offIn,left,&err);
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
      localCtx->state=TcpProcessLoginContext::STATE::PWD;
      localCtx->offIn=0;
      goto state_again;
    }
    case TcpProcessLoginContext::STATE::PWD:{
      int left=localCtx->login.crypwdLen-localCtx->offIn;
      int err;
      int n=conn->read((char*)localCtx->crypwd+localCtx->offIn,left,&err);
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
      string plainpwd=RSA::decryptWithRand16(localCtx->crypwd,g_keypriv);
      TcpProtocol::RespLogin*outresp;
      TcpProcessContext::DefaultOutCtx*outctx;      
      do{
	if(ctx->inFreq.testAndUpdate(ResourceLimit::Frequency::DIMENSION_LOGIN)){
	  outresp=new TcpProtocol::RespLogin;
	  outresp->code=TcpProtocol::RETCODE::ERR_RLIMIT_FREQ;
	  ctx->retIn|=TcpProcessContext::RETCODE::RLIMIT_FREQ;
	  break;
	}
	if(ctx->pubkey.str.empty()){
	  outresp=new TcpProtocol::RespLogin;	  
	  outresp->code=TcpProtocol::RETCODE::ERR_NO_PUBKEY;
	  break;
	}
	resource_id_t pwdId,uId,nId;
	Resource::ERRNO err;
	Resource::ScopeMutex rsm(g_resource);
	if(g_resource->isErr(err=g_resource->selectNameResource(localCtx->name,&nId))||g_resource->isErr(err=g_resource->selectUserResourceByNameId(&uId,nId))||g_resource->isErr(err=g_resource->selectPasswdResource(plainpwd,&pwdId))||g_resource->isErr(err=g_resource->isUserRefResource(uId,pwdId))){
	  outresp=new TcpProtocol::RespLogin;	  
	  outresp->code|=TcpProtocol::RETCODE::ERR;
	  break;
	}
	srand(time(nullptr));
	ctx->loginToken.date=getCurUsec();
	ctx->loginToken.uId=uId;
	ctx->loginToken.rand=rand();	
	ctx->loginToken.hton();
	string crypToken=RSA::cryptWithRand16(&ctx->loginToken,sizeof(ctx->loginToken),ctx->pubkey);
	ctx->loginToken.ntoh();	
	outresp=(TcpProtocol::RespLogin*)(new char[sizeof(TcpProtocol::RespLogin)+crypToken.size()]);
	outresp->type=TcpProtocol::Head::TYPE::RESP_LOGIN;
	outresp->code=TcpProtocol::RETCODE::OK;
	outresp->cryptlen=crypToken.size();
	memcpy(outresp+1,crypToken.c_str(),crypToken.size());
	outresp->hton();
	outctx=new TcpProcessContext::DefaultOutCtx(outresp,sizeof(TcpProtocol::RespLogin)+crypToken.size(),[](TcpProcessContext::DefaultOutCtx*ctx){
	  delete[](char*)ctx->m_data;
	});
	goto after_construct_resp;
      }while(0);
      outresp->hton();
      outctx=new TcpProcessContext::DefaultOutCtx(outresp,sizeof(TcpProtocol::RespLogin),[](TcpProcessContext::DefaultOutCtx*ctx){
	delete(TcpProtocol::RespLogin*)ctx->m_data;
      });
      after_construct_resp:
      localCtx->outNode=new TcpProcessContext::List::Node(TcpProcessContext::STATE_OUT::SEND_FIFO_GROUP,outctx,[](TcpProcessContext::List::Node*node){
	delete(TcpProcessContext::DefaultOutCtx*)node->ctx;
      });
      localCtx->state=TcpProcessLoginContext::STATE::OUT_NODE;
      goto state_again;
    }
      break;
    case TcpProcessLoginContext::STATE::OUT_NODE:{
      ctx->retIn|=TcpProcessContext::RETCODE::OUT_AGAIN;      
      if(ctx->outList.push_back(localCtx->outNode)){
	ctx->stateIn=TcpProcessContext::STATE_IN::START;
	ctx->retIn|=TcpProcessContext::RETCODE::NEXT;
	ctx->ctxIn.erase(TcpProcessContext::STATE_IN::LOGIN);
	localCtx->outNode=0;
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
  TCP_PROCESS_REGISTE_IN(login,TcpProcessContext::STATE_IN::LOGIN,handleInProcessLogin);
}

#endif
