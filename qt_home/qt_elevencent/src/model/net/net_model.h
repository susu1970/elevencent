#ifndef NET_MODEL_H_
#define NET_MODEL_H_
#include<string>
#include"rsa.hpp"
#include"net_protocol.hpp"
#include<functional>
#include<QString>
#include"global.h"
#include<unordered_set>
#include<iomanip>
#include<algorithm>
namespace qt_elevencent{
  class NetModel{
  public:
    enum class STATE_OUT{
      REQUEST_PUBKEY,
      SIGN_UP,
      LOGIN,
      RESP_PUBKEY,
      USER_SEND_MSG,
      USER_REQ_MSG,
      REQ_MSG_USERS,            
      START,
    };
    enum class STATE_IN{
      HEAD,
      RESP_PUBKEY,
      RESP_SIGN_UP,
      RESP_LOGIN,
      RESP_USER_SEND_MSG,
      RESP_USER_REQ_MSG,
      RESP_USER_REQ_MSG_FRAG,
      RESP_MSG_USERS_CODE,                                    
      RESP_MSG_USERS_FRAG,                              
      START=HEAD,
    };
    enum RETCODE{
      OK=0,
      UNKNOWN_STATE=1<<1,
      INVALID_PACKET=1<<2,      
      AGAIN_WRITE=1<<3,
      AGAIN_IN=1<<4,
    };
    class StateCtx{
    public:
#define NET_PROCESS_MAX_STATE 300          
      void*m_data;
      int m_dataLen;
      int m_dealLen=0;
      std::function<void(StateCtx*)>m_onDestroy;
      StateCtx(void*data,int dataLen,std::function<void(StateCtx*)>&&onDestroy=[](StateCtx*){}):m_data(data),m_dataLen(dataLen),m_onDestroy(std::forward<std::function<void(StateCtx*)>>(onDestroy)){}
      ~StateCtx(){m_onDestroy(this);}
    };
    class Server{
    public:
      class Host{
      public:
	std::string addr="";
	uint16_t port;
	RSA::Key rsaKeypub;
	TcpProtocol::RespLogin::Token loginToken;
	
	bool isLogin=false;
	bool isConn=false;
	void onConnected(){
	  isConn=true;
	}
	void onDisConnected(){
	  isConn=false;
	  isLogin=false;
	  rsaKeypub.str="";
	}
	Host(std::string&&addr_,uint16_t port_):addr(std::forward<std::string>(addr_)),port(port_){}
	Host(std::string&addr_,uint16_t port_):addr(std::forward<std::string>(addr_)),port(port_){}
	Host(){}	
	std::string str(){
	  return "addr: "+addr+", port: "+std::to_string(port)+"\n";
	}
	Host&operator=(const Host&h){
	  if(this==&h)
	    return *this;
	  addr=h.addr;
	  port=h.port;
	  rsaKeypub=h.rsaKeypub;
	  loginToken=h.loginToken;
	  isLogin=h.isLogin;
	  isConn=h.isConn;
	  return *this;
	}
	Host(const Host&h){
	  *this=h;
	}
	bool operator==(const Host&h){
	  if(addr==h.addr&&port==h.port)
	    return true;
	  return false;
	}
	bool operator!=(const Host&h){
	  return !(*this==h);
	}
      };
    };
    class Tx{
    public:
      STATE_OUT m_state;
      StateCtx*m_ctx;
    public:
      Tx(STATE_OUT state,StateCtx*ctx):m_state(state),m_ctx(ctx){}
    };
  };
}

#endif
