#ifndef _ELEVENCENT_PROCESS_H_
#define _ELEVENCENT_PROCESS_H_
#include<string>
#include<unordered_map>
#include<list>

#include"process_interface.h"
#include"epoll.h"
#include"connection.h"
#include"rsa.h"
namespace elevencent{
  typedef void*(*processHandleFunc_t)(void*);
  typedef void (*processHandleFuncCb_t)(void*);    
  class TcpProcessContext{
  public:
    static constexpr uint32_t ms_bufInSize=2048;
    static constexpr uint32_t ms_bufOutSize=2048;    
    enum class RETCODE_IN:int8_t{
      CLOSE,
      OK,
      NEXT,
    };
    enum class RETCODE_OUT:int8_t{
      CLOSE,
      OK,
      NEXT,
    };
    enum class STATE_IN:uint16_t{
      HEAD,
      INSERT_USER_RESOURCE,
      INSERT_PASSWD_RESOURCE,
      USER_REF_RESOURCE,
      REQUEST_PUBKEY,

      START=HEAD,
    };
    enum class STATE_OUT:uint16_t{
      REQUEST_PUBKEY,
      
      START=REQUEST_PUBKEY,
    };
    char bufIn[ms_bufInSize];
    int offIn;
    char bufOut[ms_bufOutSize];
    int offOut;    
    STATE_IN stateIn,stateInCb;
    STATE_OUT stateOut,stateOutCb;
    RETCODE_IN retIn;
    RETCODE_OUT retOut;
    Epoll*ep;
    std::unordered_map<STATE_IN,void*>ctxIn;
    std::unordered_map<STATE_IN,void*>ctxOut;
    RSA::Key*rsaPubkey,*rsaPrvkey;    
    TcpProcessContext(Epoll*ep,RSA::Key*pubkey,RSA::Key*prvkey);
    void registeDestroy(std::function<void(void*arg)>&&func);
    ~TcpProcessContext();
  private:
    std::list<std::function<void(void*arg)>>destroyFuncs;
  };
  class TcpProcess:public ProcessInterface{
  public:
    virtual void*handleIn(void*arg);
    virtual void handleInCb(void*arg);
    virtual void*handleOut(void*arg);
    virtual void handleOutCb(void*arg);
    static void*defaultHandleIn(void*arg);
    static void defaultHandleInCb(void*arg);
    static void*defaultHandleOut(void*arg);
    static void defaultHandleOutCb(void*arg);        
    class Factory{
      Factory()=delete;
      Factory(const Factory&)=delete;
      Factory(const Factory&&)=delete;      
      Factory&operator=(const Factory&)=delete;
      static std::unordered_map<TcpProcessContext::STATE_IN,processHandleFunc_t>ms_handleInFuncUmap;
      static std::unordered_map<TcpProcessContext::STATE_IN,processHandleFuncCb_t>ms_handleInCbFuncUmap;
      static std::unordered_map<TcpProcessContext::STATE_OUT,processHandleFunc_t>ms_handleOutFuncUmap;
      static std::unordered_map<TcpProcessContext::STATE_OUT,processHandleFuncCb_t>ms_handleOutCbFuncUmap;      
    public:
      static processHandleFunc_t getHandle(const TcpProcessContext::STATE_IN state);
      static processHandleFuncCb_t getHandleCb(const TcpProcessContext::STATE_IN state);
      static processHandleFunc_t getHandle(const TcpProcessContext::STATE_OUT state);
      static processHandleFuncCb_t getHandleCb(const TcpProcessContext::STATE_OUT state);
      class Registe{
      public:
	Registe(const TcpProcessContext::STATE_IN state,processHandleFuncCb_t cb,processHandleFunc_t func=TcpProcess::defaultHandleIn);
	Registe(const TcpProcessContext::STATE_IN state,processHandleFunc_t func=TcpProcess::defaultHandleIn,processHandleFuncCb_t cb=TcpProcess::defaultHandleInCb);
	Registe(const TcpProcessContext::STATE_OUT state,processHandleFuncCb_t cb,processHandleFunc_t func=TcpProcess::defaultHandleOut);	
	Registe(const TcpProcessContext::STATE_OUT state,processHandleFunc_t func=TcpProcess::defaultHandleOut,processHandleFuncCb_t cb=TcpProcess::defaultHandleOutCb);
      };
    };
  };
  void*TcpProcess::defaultHandleIn(void*arg){
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    ctx->retIn=TcpProcessContext::RETCODE_IN::CLOSE;
    return arg;
  }
  void TcpProcess::defaultHandleInCb(void*arg){
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    switch(ctx->retIn){
    case TcpProcessContext::RETCODE_IN::CLOSE:
      delete conn;
      return;
    case TcpProcessContext::RETCODE_IN::OK:
      return;
    case TcpProcessContext::RETCODE_IN::NEXT:
      ctx->stateIn=ctx->stateInCb;
      conn->handleInCb(conn->handleIn(arg));
      return;
    default:
      delete conn;
      return;
    }
  }
  void*TcpProcess::defaultHandleOut(void*arg){
    return arg;
  }
  void TcpProcess::defaultHandleOutCb(void*arg){
    
  }
#define TCP_PROCESS_REGISTE_IN(name,state,...) static TcpProcess::Factory::Registe s_tcpProcessFactoryRegisteIn##name(state,##__VA_ARGS__)
#define TCP_PROCESS_REGISTE_OUT(name,state,...) static TcpProcess::Factory::Registe s_tcpProcessFactoryRegisteOut##name(state,##__VA_ARGS__)
}

#endif
