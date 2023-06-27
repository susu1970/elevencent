#ifndef TCP_PROCESS_SIGN_UP_HPP_
#define  TCP_PROCESS_SIGN_UP_HPP_

#include"process.h"
#include"global.h"
#include"resource_global.h"
#include"resource.h"
#include"rsa.h"

namespace elevencent{
  class TcpProcessSignUpContext{
  public:
    int offIn=0;
    char*name=0;
    char*crypwd=0;
    enum STATE{
      SIGNUP,
      NAME,
      PWD,
      OUT_NODE,
    };
    STATE state=SIGNUP;
    TcpProtocol::SignUp signup;
    TcpProcessContext::List::Node*outNode=0;    
    ~TcpProcessSignUpContext(){
      if(name)
	delete[]name;
      if(crypwd)
	delete[]crypwd;
      if(outNode)
	delete outNode;
    }
  };
  void*handleInProcessSignUp(void*arg){
    using namespace std;
    using namespace ResourceLimit;
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    ctx->retIn=TcpProcessContext::RETCODE::OK;    
    TcpProcessSignUpContext*localCtx=(TcpProcessSignUpContext*)ctx->ctxIn[TcpProcessContext::STATE_IN::SIGN_UP];
    if(!localCtx){
      ctx->ctxIn[TcpProcessContext::STATE_IN::SIGN_UP]=localCtx=new TcpProcessSignUpContext;
      ctx->registeOnDestroyFunc(TcpProcessContext::STATE_IN::SIGN_UP,[](void*arg){
	TcpProcessContext*ctx=(TcpProcessContext*)arg;
	TcpProcessSignUpContext*localCtx=(TcpProcessSignUpContext*)ctx->ctxIn[TcpProcessContext::STATE_IN::SIGN_UP];
	if(localCtx)
	  delete localCtx;
	ctx->ctxIn.erase(TcpProcessContext::STATE_IN::SIGN_UP);
      });
    }
  state_again:
    switch(localCtx->state){
    case TcpProcessSignUpContext::STATE::SIGNUP:{
      int left=sizeof(TcpProtocol::SignUp)-localCtx->offIn;
      int err;
      int n=conn->read((char*)&localCtx->signup+localCtx->offIn,left,&err);
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
      localCtx->signup.ntoh();
      if(localCtx->signup.nameLen>g_sizeLimit["name_resource_name"]||localCtx->signup.crypwdLen>g_sizeLimit["tcp_process_max_cryped_size"]){
	DEBUG_PRETTY_MSG("length error");
	ctx->retIn|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
	return arg;
      }
      localCtx->name=new char[localCtx->signup.nameLen+1];
      localCtx->name[localCtx->signup.nameLen]=0;
      localCtx->crypwd=new char[localCtx->signup.crypwdLen+1];
      localCtx->crypwd[localCtx->signup.crypwdLen]=0;
      localCtx->state=TcpProcessSignUpContext::STATE::NAME;
      localCtx->offIn=0;
      goto state_again;
    }
      break;
    case TcpProcessSignUpContext::STATE::NAME:{
      int left=localCtx->signup.nameLen-localCtx->offIn;
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
      localCtx->state=TcpProcessSignUpContext::STATE::PWD;
      localCtx->offIn=0;
      goto state_again;
    }
    case TcpProcessSignUpContext::STATE::PWD:{
      int left=localCtx->signup.crypwdLen-localCtx->offIn;
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
      if(left>0)
	return arg;
      string plainpwd=RSA::decryptWithRand16(localCtx->crypwd,g_keypriv);
      if(plainpwd.size()>g_sizeLimit["passwd_resource_passwd"]){
	DEBUG_PRETTY_MSG("length error");
	ctx->retIn|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
	return arg;
      }
      
      TcpProtocol::RespSignup*outresp=new TcpProtocol::RespSignup();
      do{
	if(ctx->inFreq.testAndUpdate(ResourceLimit::Frequency::DIMENSION_SIGN_UP)){
	  outresp->code=TcpProtocol::RETCODE::ERR_RLIMIT_FREQ;
	  ctx->retIn|=TcpProcessContext::RETCODE::RLIMIT_FREQ;
	  break;
	}		
	resource_id_t pwdId,uId,nId;
	Resource::ScopeMutex rsm(g_resource);
	Resource::ERRNO err=g_resource->insertNameResource(localCtx->name,&nId);
	if(g_resource->isErr(err)){
	  outresp->code|=TcpProtocol::RETCODE::ERR_RESOURCE;
	  break;
	}
	if(g_resource->isWarn(err)){
	  if(err&Resource::WARN_RESOURCE_EXIST){
	    outresp->code|=TcpProtocol::RETCODE::WARN_RESOURCE_EXIST;
	    break;
	  }	  
	  outresp->code|=TcpProtocol::RETCODE::WARN_RESOURCE;
	}	
	err=g_resource->insertPasswdResource(plainpwd,&pwdId);
	if(g_resource->isErr(err)){
	  outresp->code|=TcpProtocol::RETCODE::ERR_RESOURCE;
	  break;
	}
	if(g_resource->isWarn(err))
	  outresp->code|=TcpProtocol::RETCODE::WARN_RESOURCE;
	err=g_resource->insertUserResource(&uId);
	if(g_resource->isErr(err)){
	  outresp->code|=TcpProtocol::RETCODE::ERR_RESOURCE;
	  break;
	}
	if(g_resource->isWarn(err))
	  outresp->code|=TcpProtocol::RETCODE::WARN_RESOURCE;
	err=g_resource->userRefResource(uId,nId);
	if(g_resource->isErr(err)){
	  outresp->code|=TcpProtocol::RETCODE::ERR_RESOURCE;
	  break;
	}
	if(g_resource->isWarn(err)){
	  outresp->code|=TcpProtocol::RETCODE::WARN_RESOURCE;
	}
	err=g_resource->userRefResource(uId,pwdId);
	if(g_resource->isErr(err)){
	  outresp->code|=TcpProtocol::RETCODE::ERR_RESOURCE;
	  break;
	}
	if(g_resource->isWarn(err))
	  outresp->code|=TcpProtocol::RETCODE::WARN_RESOURCE;
	outresp->uId=uId;
      }while(0);
      outresp->hton();
      TcpProcessContext::DefaultOutCtx*outctx=new TcpProcessContext::DefaultOutCtx(outresp,sizeof(TcpProtocol::RespSignup),[](TcpProcessContext::DefaultOutCtx*ctx){
	delete(TcpProtocol::RespSignup*)ctx->m_data;
      });
      localCtx->outNode=new TcpProcessContext::List::Node(TcpProcessContext::STATE_OUT::SEND_FIFO_GROUP,outctx,[](TcpProcessContext::List::Node*node){
	delete (TcpProcessContext::DefaultOutCtx*)node->ctx;
      });
      localCtx->state=TcpProcessSignUpContext::STATE::OUT_NODE;
      goto state_again;
    }
      break;
    case TcpProcessSignUpContext::STATE::OUT_NODE:{
      ctx->retIn|=TcpProcessContext::RETCODE::OUT_AGAIN;      
      if(ctx->outList.push_back(localCtx->outNode)){
	ctx->retIn|=TcpProcessContext::RETCODE::NEXT;
	ctx->stateIn=TcpProcessContext::STATE_IN::START;
	localCtx->outNode=0;
	delete localCtx;
	ctx->ctxIn.erase(TcpProcessContext::STATE_IN::SIGN_UP);
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
  TCP_PROCESS_REGISTE_IN(sign_up,TcpProcessContext::STATE_IN::SIGN_UP,handleInProcessSignUp);
}

#endif
