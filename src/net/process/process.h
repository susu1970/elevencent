#ifndef _ELEVENCENT_PROCESS_H_
#define _ELEVENCENT_PROCESS_H_
#include"process_interface.h"
#include"epoll.h"
#include"connection.h"
#include<string>
#include<unordered_map>
namespace elevencent{
  typedef void*(*processHandleFunc_t)(void*);
  typedef void (*processHandleFuncCb_t)(void*);    
  class TcpProcessContext{
  public:
    static constexpr uint32_t ms_bufSize=2048;
    enum RETCODE:int8_t{
      CLOSE,
      OK,
      NEXT_HANDLE,
    };
    enum STATE:uint16_t{
      HEAD,
      INSERT_USER_RESOURCE,
      INSERT_PASSWD_RESOURCE,
      USER_REF_RESOURCE,
      REQUEST_PUBKEY,

      START=HEAD,
    };
    char buf[ms_bufSize];
    int off;
    STATE state,stateCb;
    Epoll*ep;
    RETCODE retcode;
    TcpProcessContext(Epoll*ep=0);
    ~TcpProcessContext();
  };
  class TcpProcess:public ProcessInterface{
  public:
    virtual void*handleIn(void*arg);
    virtual void handleInCb(void*arg);
    virtual void*handleOut(void*arg);
    virtual void handleOutCb(void*arg);
    class Factory{
      Factory()=delete;
      Factory(const Factory&)=delete;
      Factory(const Factory&&)=delete;      
      Factory&operator=(const Factory&)=delete;
      static std::unordered_map<TcpProcessContext::STATE,processHandleFunc_t>ms_handleInFuncUmap;
      static std::unordered_map<TcpProcessContext::STATE,processHandleFuncCb_t>ms_handleInCbFuncUmap;
      static std::unordered_map<TcpProcessContext::STATE,processHandleFunc_t>ms_handleOutFuncUmap;
      static std::unordered_map<TcpProcessContext::STATE,processHandleFuncCb_t>ms_handleOutCbFuncUmap;      
    public:
      static processHandleFunc_t getHandleIn(const TcpProcessContext::STATE state);
      static processHandleFuncCb_t getHandleInCb(const TcpProcessContext::STATE state);
      static processHandleFunc_t getHandleOut(const TcpProcessContext::STATE state);
      static processHandleFuncCb_t getHandleOutCb(const TcpProcessContext::STATE state);
      static void registeHandleIn(const TcpProcessContext::STATE state,processHandleFunc_t func);
      static void registeHandleInCb(const TcpProcessContext::STATE state,processHandleFuncCb_t func);
      static void registeHandleOut(const TcpProcessContext::STATE state,processHandleFunc_t func);
      static void registeHandleOutCb(const TcpProcessContext::STATE state,processHandleFuncCb_t func);
      static void registe(const TcpProcessContext::STATE state,processHandleFunc_t in,processHandleFuncCb_t inCb,processHandleFunc_t out,processHandleFuncCb_t outCb);
      class Registe{
      public:
	Registe(const TcpProcessContext::STATE state,processHandleFunc_t in,processHandleFuncCb_t inCb,processHandleFunc_t out,processHandleFuncCb_t outCb);
      };
    };
  };
#define TCP_PROCESS_REGISTE(name,state,in,inCb,out,outCb) static TcpProcess::Factory::Registe s_tcpProcessFactoryRegiste##name(state,in,inCb,out,outCb);
}

#endif
