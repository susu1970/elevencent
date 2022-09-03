#ifndef _ELEVENCENT_PROCESS_H_
#define _ELEVENCENT_PROCESS_H_
#include<string>
#include<unordered_map>
#include<list>

#include"rsa.h"
#include"spinlock.hpp"
#include"process_interface.h"
#include"epoll.h"
#include"connection.h"
namespace elevencent{
  typedef void*(*processHandleFunc_t)(void*);
  typedef void (*processHandleFuncCb_t)(void*);
  class TcpProcessContext{
  public:
    enum RETCODE{
      OK=1<<1,
      NEXT=1<<2,
      SHUT_RD=1<<3,
      SHUT_WR=1<<4,
      CLOSE=1<<5,
      EP_IN=1<<6,
      EP_OUT=1<<7
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
      
      START,
    };
    STATE_IN stateIn,stateInCb;
    STATE_OUT stateOut,stateOutCb;
    RETCODE retIn,retOut;
    Epoll*ep;
    int events;
    std::unordered_map<STATE_IN,void*>ctxIn;
    std::list<std::pair<STATE_OUT,void*>>outList;
    SpinLock outLock;
    TcpProcessContext(int events,Epoll*ep);
    void registeOnDestroyFunc(std::function<void(void*arg)>&&func);
    ~TcpProcessContext();
  private:
    std::list<std::function<void(void*arg)>>onDestroyFuncs;
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
  inline TcpProcessContext::RETCODE operator&(TcpProcessContext::RETCODE a,TcpProcessContext::RETCODE b){
    return (TcpProcessContext::RETCODE)(((int)a)&((int)b));
  }
  inline TcpProcessContext::RETCODE operator|(TcpProcessContext::RETCODE a,TcpProcessContext::RETCODE b){
    return (TcpProcessContext::RETCODE)(((int)a)|((int)b));
  }
  inline TcpProcessContext::RETCODE operator~(TcpProcessContext::RETCODE a){
    return (TcpProcessContext::RETCODE)(~((int)a));
  }
  inline TcpProcessContext::RETCODE operator&=(TcpProcessContext::RETCODE&a,TcpProcessContext::RETCODE b){
    return a=a&b;
  }  
  inline TcpProcessContext::RETCODE operator|=(TcpProcessContext::RETCODE&a,TcpProcessContext::RETCODE b){
    return a=a|b;
  }  
  void*TcpProcess::defaultHandleIn(void*arg){
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    ctx->retIn=TcpProcessContext::RETCODE::CLOSE;
    return arg;
  }
  void TcpProcess::defaultHandleInCb(void*arg){
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    if((ctx->retIn&TcpProcessContext::RETCODE::CLOSE)
       ||(ctx->retIn&TcpProcessContext::RETCODE::SHUT_RD))
      return;    
    if(ctx->retIn&TcpProcessContext::RETCODE::NEXT){
      ctx->retIn&=(~TcpProcessContext::RETCODE::NEXT);
      ctx->stateIn=ctx->stateInCb;
      conn->handleInCb(conn->handleIn(arg));
    }
  }
  void*TcpProcess::defaultHandleOut(void*arg){
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    ctx->retOut=TcpProcessContext::RETCODE::CLOSE;    
    return arg;
  }
  void TcpProcess::defaultHandleOutCb(void*arg){
    TcpConnection*conn=(TcpConnection*)arg;
    TcpProcessContext*ctx=(TcpProcessContext*)conn->ctx;
    if((ctx->retOut&TcpProcessContext::RETCODE::CLOSE)
       ||(ctx->retOut&TcpProcessContext::RETCODE::SHUT_RD)
       ||(ctx->retOut&TcpProcessContext::RETCODE::SHUT_WR))
      return;
    if(ctx->retOut&TcpProcessContext::RETCODE::NEXT){
      ctx->retOut&=(~TcpProcessContext::RETCODE::NEXT);
      ctx->stateOut=ctx->stateOutCb;
      conn->handleOutCb(conn->handleOut(arg));
    }
  }
#define TCP_PROCESS_REGISTE_IN(name,state,...) static TcpProcess::Factory::Registe s_tcpProcessFactoryRegisteIn##name(state,##__VA_ARGS__)
#define TCP_PROCESS_REGISTE_OUT(name,state,...) static TcpProcess::Factory::Registe s_tcpProcessFactoryRegisteOut##name(state,##__VA_ARGS__)
  const RSA::Key gk_rsaPubkey("0082010etehrkr6goj4kpbgivsntorkrdfkc3il9rp8r1ab1q57i27v6abkemfjdm1jsde02oj81lrhc9kqk93s70t87hgec15pc3gslnpnpbrkjscl8c7vqvrr45mevd3hsfa647qig9gipmkjp1hl2i10kt48p6d941delp7v0uibenojo264opprtqnvpf0m6erujl1df2n2ng78qmaa976knn2tjhodnd1kdmqen4a40fijo4l7s45873gi8s2u6bvcpmmk5vbqie0m3n6og8v8ruhlentnbjvtpn371tbd75tkjqg4ga9cpnkdpp62rn472fi8eo9011n17g2crlqh8h8oppfcu1sa7aqb6o7fdkcb4bcj1joakge7u0afv918recl55tu0svn6pgqe10e7g8odjp6egonvbf8ku2b9vhqgj1ahdg7vtc56m9u6bsvm0skevb1sk4d2bla47me0sfp1tphibvivhb9b8gb8geko29ps4lb58c4u0p2c02c64rueu9o8thsa4cevo297trvustbqtkucj6d0lsfmu4bdt9smeeu3hh1j1gakgc3a3c2d1gta0tnba5gv5gfsu4d4pofrjmgrv4hqpq27jjjqr42trr6saqn0blebvslmb6s1ggciug7viqaompoi26vff0uh28pgj6vgn5u1i611np746fs6k6eqhvn6mm63ochniunjlt777frs8lut7dcg1a0jd9fu3dckqta0clhhgajvmid5ms1feso3f130tnfcvr5da3910474v8rkgj9vlfre8qamv6lnurj04nfvdm5k9000042001");
  const RSA::Key gk_rsaPrvkey("0082010etehrkr6goj4kpbgivsntorkrdfkc3il9rp8r1ab1q57i27v6abkemfjdm1jsde02oj81lrhc9kqk93s70t87hgec15pc3gslnpnpbrkjscl8c7vqvrr45mevd3hsfa647qig9gipmkjp1hl2i10kt48p6d941delp7v0uibenojo264opprtqnvpf0m6erujl1df2n2ng78qmaa976knn2tjhodnd1kdmqen4a40fijo4l7s45873gi8s2u6bvcpmmk5vbqie0m3n6og8v8ruhlentnbjvtpn371tbd75tkjqg4ga9cpnkdpp62rn472fi8eo9011n17g2crlqh8h8oppfcu1sa7aqb6o7fdkcb4bcj1joakge7u0afv918recl55tu0svn6pgqe10e7g8odjp6egonvbf8ku2b9vhqgj1ahdg7vtc56m9u6bsvm0skevb1sk4d2bla47me0sfp1tphibvivhb9b8gb8geko29ps4lb58c4u0p2c02c64rueu9o8thsa4cevo297trvustbqtkucj6d0lsfmu4bdt9smeeu3hh1j1gakgc3a3c2d1gta0tnba5gv5gfsu4d4pofrjmgrv4hqpq27jjjqr42trr6saqn0blebvslmb6s1ggciug7viqaompoi26vff0uh28pgj6vgn5u1i611np746fs6k6eqhvn6mm63ochniunjlt777frs8lut7dcg1a0jd9fu3dckqta0clhhgajvmid5ms1feso3f130tnfcvr5da3910474v8rkgj9vlfre8qamv6lnurj04nfvdm5k900819bd54fsoduh1l4tvf6jha24q2q7dufkhthdpikp7mc57cjnvsghcuadp4jtolh48ssatmc67jmh3lr1l6g423lh9tlgveiiqeucissumjg70rp7oo137mc5m67f629teaknguck5mfosfr7mac63g20gl3hm8djg2bhi9icq15nfbsb393ej4lo4k78gdrs01sb6a17p4a3cilcgqb3gpputau4n6niesmlnqcgktfntdc8icuborutkrhqcldn5kdn8gim2d33b9o039qo1ab4n3uhfo8r4a4j4f2bdvdscilarjuil97jle3lfv1994dh76mkl0m1507v2lon6h1od69594259jophrpa47u6qnlcu9kalsgbbm7aas5tfipd8735v48rd6ku1ql8melic0mfvcempun7bi34so466doc29dmu8parmbpggh26av62mdvvrhmi8vft05jtei14es0bujmf871am0710nrdt7vh0q76l32r8a84dvjipcer5jfu5v1t30rn5djar8c74q4e4de5lmc2a3p5j0l5k1jlrp1k10m0l5tn79iiam4v4oh7cbb8q4o2hg7l19mikvpgvds16uuq6btgk4157acvokl5q2hi1kr8hjo9igq4gmiv5l76pnh621rns25u25oteq0oe3pv3j9igcg71v5sbcc6kp6lun46dqodi6a0bobrkbd5ggs2l04bdjuef3ec3s5vrmacscehttbk48j3njd4cf8kckegdu5kinikvrtqhs8phk1c9it5j22u5ipksr3ndcljbv45ravkt4hp6ro1");    
}

#endif
