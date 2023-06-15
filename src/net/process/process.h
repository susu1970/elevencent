#ifndef _ELEVENCENT_PROCESS_H_
#define _ELEVENCENT_PROCESS_H_
#include<string>
#include<unordered_map>
#include<list>
#include<atomic>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<unistd.h>

#include"rsa.h"
#include"spinlock.hpp"
#include"process_interface.h"
#include"epoll.h"
#include"connection.h"
#include"protocol.hpp"

namespace elevencent{
  typedef void*(*processHandleFunc_t)(void*);
  typedef void (*processHandleFuncCb_t)(void*);
  class TcpProcessContext{
  public:
    enum RETCODE{
      OK=0,
      NEXT=1,
      IN_CLOSE=1<<1,
      IN_AGAIN=1<<2,
      OUT_CLOSE=1<<3,
      OUT_AGAIN=1<<4,
      RLIMIT_QOS=1<<5,
      RLIMIT_CPU=1<<6,
      RLIMIT_FREQ=1<<7,
      RLIMIT_SIZE=1<<10,
      IN_INSERT_EV=1<<8,
      OUT_INSERT_EV=1<<9,            
    };
    enum class STATE_IN{
      HEAD=1,
      INSERT_USER_RESOURCE,
      INSERT_PASSWD_RESOURCE,
      USER_REF_RESOURCE,
      REQUEST_PUBKEY,//请求公钥
      SIGN_UP,
      LOGIN,
      RESP_PUBKEY,
      USER_SEND_MSG,
      USER_REQ_MSG,
      REQ_MSG_USERS,      
      START=HEAD,
    };
    enum class STATE_OUT{
      RESP_PUBKEY,
      SEND_FIFO_GROUP,
      FRAG,                                    
      START,
    };    
    class OutFragCtx{
      std::function<void(OutFragCtx*)>onDestroy;            
    public:
      std::function<ssize_t(OutFragCtx*fragCtx,void*buf,size_t count)>read;
      void*priv=0;
      OutFragCtx(std::function<ssize_t(OutFragCtx*fragCtx,void*buf,size_t count)>&&read_,std::function<void(OutFragCtx*)>&&onDestroy_,void*priv_=0):onDestroy(std::forward<std::function<void(OutFragCtx*)>>(onDestroy_)),priv(priv_),read(std::forward<std::function<ssize_t(OutFragCtx*fragCtx,void*buf,size_t count)>>(read_)){}
      ~OutFragCtx(){onDestroy(this);}
    };
    class DefaultOutCtx{
    public:
      void*m_data;
      int m_dealLen=0;
      int m_dataLen;
      std::function<void(DefaultOutCtx*)>m_onDestroy;
      DefaultOutCtx(void*data,int dataLen,std::function<void(DefaultOutCtx*)>&&onDestroy=[](DefaultOutCtx*){}):m_data(data),m_dataLen(dataLen),m_onDestroy(std::forward<std::function<void(DefaultOutCtx*)>>(onDestroy)){}
      ~DefaultOutCtx(){m_onDestroy(this);}      
    };
    class List{
    public:
      volatile std::atomic<size_t>fuzzyMaxSize=-1ul;
      class Node{
	friend List;
	Node(){}
	std::function<void(Node*)>onDestroy=[](Node*){};
      public:
	Node*next=0;
	STATE_OUT stateOut;
	void*ctx;
	Node(STATE_OUT stateOut_,void*ctx_,std::function<void(Node*)>onDestroy_=[](Node*){}):stateOut(stateOut_),ctx(ctx_),onDestroy(std::forward<std::function<void(Node*)>>(onDestroy_)){}
	~Node(){onDestroy(this);}
      };
      List();
      ~List();
      bool push_back(Node*);
      bool rm(Node*);
      Node*front();
      size_t size();
    private:
      volatile Node*head,*tail;
      volatile std::atomic<size_t>fuzzySize=0;
    };
#define TCP_PROCESS_CONTEXT_MAX_STATE 300
    STATE_IN stateIn=STATE_IN::START,stateInCb=STATE_IN::START;
    STATE_OUT stateOut=STATE_OUT::START,stateOutCb=STATE_OUT::START;
    ResourceLimit::Frequency inFreq,outFreq;
    int retIn=RETCODE::OK,retOut=RETCODE::OK;    
    std::unordered_map<STATE_IN,void*>ctxIn;
    std::unordered_map<STATE_OUT,void*>ctxOut;    
    List outList;
    List outFragList;
    uint8_t outStartTurn=0;
    uint32_t outFragTurn=0;
    void registeOnDestroyFunc(std::function<void(void*arg)>&&func);
    void registeOnDestroyFunc(STATE_IN state,std::function<void(void*arg)>&&func,bool doOld=false);
    void registeOnDestroyFunc(STATE_OUT state,std::function<void(void*arg)>&&func,bool doOld=false);    
    TcpProtocol::RespLogin::Token loginToken;
    RSA::Key pubkey;
    ~TcpProcessContext();
  private:
    std::unordered_map<STATE_IN,std::function<void(void*arg)>>onDestroyFuncsIn;
    std::unordered_map<STATE_OUT,std::function<void(void*arg)>>onDestroyFuncsOut;
    std::list<std::function<void(void*arg)>>onDestroyFuncs;
    SpinLock onDestroyFuncsLock;
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
    public:
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
    ctx->retIn=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
    return arg;
  }
  void TcpProcess::defaultHandleInCb(void*arg){
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    if(conn->inCpu&&conn->inCpu->testAndUpdate())
      ctx->retIn|=TcpProcessContext::RETCODE::RLIMIT_CPU;
    if(ctx->retIn&TcpProcessContext::RETCODE::RLIMIT_QOS||ctx->retIn&TcpProcessContext::RETCODE::RLIMIT_CPU)
      ctx->retIn|=TcpProcessContext::RETCODE::IN_INSERT_EV;
    if(ctx->retIn&TcpProcessContext::RETCODE::OUT_AGAIN||ctx->retIn&TcpProcessContext::RETCODE::OUT_INSERT_EV||ctx->retIn&TcpProcessContext::RETCODE::OUT_CLOSE)//为了激活out回调，返回后再次进入in回调
      ctx->retIn|=TcpProcessContext::RETCODE::IN_AGAIN;
    if(ctx->retIn&TcpProcessContext::RETCODE::IN_CLOSE||ctx->retIn&TcpProcessContext::RETCODE::OUT_CLOSE||ctx->retIn&TcpProcessContext::RETCODE::IN_AGAIN||ctx->retIn&TcpProcessContext::RETCODE::OUT_AGAIN||ctx->retIn&TcpProcessContext::RETCODE::IN_INSERT_EV||ctx->retIn&TcpProcessContext::RETCODE::OUT_INSERT_EV)
      return;          
    if(ctx->retIn&TcpProcessContext::RETCODE::NEXT)
      conn->process->handleInCb(conn->process->handleIn(arg));
  }
  void*TcpProcess::defaultHandleOut(void*arg){
    using namespace std;
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    ctx->retOut=TcpProcessContext::RETCODE::OK;
    auto outNode=ctx->outList.front();
    if(!outNode){
      ctx->stateOut=TcpProcessContext::STATE_OUT::START;
      ctx->retOut|=TcpProcessContext::RETCODE::NEXT;    
      return arg;
    }
    TcpProcessContext::DefaultOutCtx*localCtx=(TcpProcessContext::DefaultOutCtx*)outNode->ctx;
    int left=localCtx->m_dataLen-localCtx->m_dealLen;
    int err;
    int n=conn->write((const char*)localCtx->m_data+localCtx->m_dealLen,left,&err);
    left-=n;
    localCtx->m_dealLen+=n;
    if(err&Connection::ERRNO_RLIMIT_QOS)
      ctx->retOut|=TcpProcessContext::RETCODE::RLIMIT_QOS;
    if(err&Connection::ERRNO_CLOSE){
      ctx->retOut|=TcpProcessContext::RETCODE::IN_CLOSE|TcpProcessContext::RETCODE::OUT_CLOSE;
      return arg;
    }
    if(left)
      return arg;
    ctx->outList.rm(outNode);
    delete outNode;
    ctx->stateOut=TcpProcessContext::STATE_OUT::START;
    ctx->retOut|=TcpProcessContext::RETCODE::NEXT;    
    return arg;
  }
  void TcpProcess::defaultHandleOutCb(void*arg){
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    if(conn->outCpu&&conn->outCpu->testAndUpdate())
      ctx->retOut|=TcpProcessContext::RETCODE::RLIMIT_CPU;
    if(ctx->retOut&TcpProcessContext::RETCODE::RLIMIT_QOS||ctx->retOut&TcpProcessContext::RETCODE::RLIMIT_CPU)
      ctx->retOut|=TcpProcessContext::RETCODE::OUT_INSERT_EV;    
    if(ctx->retOut&TcpProcessContext::RETCODE::IN_AGAIN||ctx->retOut&TcpProcessContext::RETCODE::IN_INSERT_EV||ctx->retOut&TcpProcessContext::RETCODE::IN_CLOSE)//为了激活in回调，返回后再次进入out回调
      ctx->retOut|=TcpProcessContext::RETCODE::OUT_AGAIN;
    if(ctx->retOut&TcpProcessContext::RETCODE::OUT_CLOSE||ctx->retOut&TcpProcessContext::RETCODE::IN_CLOSE||ctx->retOut&TcpProcessContext::RETCODE::OUT_AGAIN||ctx->retOut&TcpProcessContext::RETCODE::IN_AGAIN||ctx->retOut&TcpProcessContext::RETCODE::OUT_INSERT_EV||ctx->retOut&TcpProcessContext::RETCODE::IN_INSERT_EV)
      return;
    if(ctx->retOut&TcpProcessContext::RETCODE::NEXT)
      conn->process->handleOutCb(conn->process->handleOut(arg));
  }
#define TCP_PROCESS_REGISTE_IN(name,state,...) static TcpProcess::Factory::Registe s_tcpProcessFactoryRegisteIn##name(state,##__VA_ARGS__)
#define TCP_PROCESS_REGISTE_OUT(name,state,...) static TcpProcess::Factory::Registe s_tcpProcessFactoryRegisteOut##name(state,##__VA_ARGS__)
}

#endif
