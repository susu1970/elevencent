#ifndef ELEVENCENT_PROTOCOL_H_
#define ELEVENCENT_PROTOCOL_H_

#include<stdint.h>
#include<atomic>
#include"db_mapper.hpp"
#include"resource_global.h"
#include"global.h"

namespace elevencent{
#pragma pack(1)  
  class TcpProtocol{
  public:
    enum RETCODE:uint32_t{
      OK=0,

      ERR_START=1,

      ERR_RLIMIT_FREQ=ERR_START<<1,
      ERR_RESOURCE=ERR_START<<2,
      ERR=ERR_START<<3,
      ERR_NO_PUBKEY=ERR_START<<4,
      ERR_LOGIN_TOKEN=ERR_START<<5,

      ERR_END=ERR_START<<6,

      WARN_START=ERR_END<<1,

      WARN_RESOURCE=WARN_START<<1,
      WARN=WARN_START<<2,
      WARN_RESOURCE_EXIST=WARN_START<<3,

      WARN_END=WARN_START<<6
    };
    class Head{
    public:
      uint8_t version;
      uint16_t type;
      enum TYPE:uint16_t{
	REQUEST_PUBKEY,
	RESP_PUBKEY,
	SIGN_UP,
	RESP_SIGN_UP,
	TOAST,
	LOGIN,
	RESP_LOGIN,
	USER_SEND_MSG,
	RESP_USER_SEND_MSG,
	USER_REQ_MSG,
	RESP_USER_MSG,
	RESP_USER_REQ_MSG,	
	RESP_USER_REQ_MSG_FRAG,
	REQ_MSG_USERS,
	RESP_MSG_USERS_CODE,
	RESP_MSG_USERS_FRAG,		
      };
      void ntoh(){
	type=ntohs(type);
      }
      void hton(){
	type=htons(type);
      }
      Head(){}
      Head(TYPE type_):type(type_){}
    };
    class RespCode:public Head{
    public:
      uint32_t code=RETCODE::OK;
      RespCode(TYPE type_,RETCODE code_=RETCODE::OK):Head(type_),code(code_){}
      void hton(){
	Head::hton();
	code=elevencent::hton<uint32_t>(code);
      }
      void ntoh(){
	Head::ntoh();
	code=elevencent::ntoh<uint32_t>(code);
      }
    };
    class RespUserSendMsg:public RespCode{
    public:
      RespUserSendMsg():RespCode(RESP_USER_SEND_MSG){}
      
    };
    class RespUserReqMsg:public RespCode{
    public:
      RespUserReqMsg():RespCode(RESP_USER_REQ_MSG){}
    };
    class RespReqMsgUsers:public RespCode{
    public:
      RespReqMsgUsers():RespCode(RESP_MSG_USERS_CODE){}
    };
    class RespSignup:public RespCode{
    public:
      resource_id_t uId;
      RespSignup():RespCode(RESP_SIGN_UP){}
      void hton(){
	RespCode::hton();
	uId=elevencent::hton<resource_id_t>(uId);
      }
    };
    class RespLogin:public RespCode{
    public:
      class Token{
      public:
	uint64_t rand;
	datetime_t date;
	resource_id_t uId;
	Token(){}
	Token(const Token&r){
	  *this=r;
	}
	bool operator==(const Token&r){
	  return rand==r.rand&&date==r.date&&uId==r.uId;
	}
	bool operator!=(const Token&r){
	  return !(*this==r);
	}            	
	Token&operator=(const Token&r){
	  rand=r.rand;
	  date=r.date;
	  uId=r.uId;
	  return *this;
	}
	void hton(){
	  uId=elevencent::hton<resource_id_t>(uId);
	  rand=elevencent::hton<uint64_t>(rand);
	  date=elevencent::hton<datetime_t>(date);
	}
	void ntoh(){
	  uId=elevencent::ntoh<resource_id_t>(uId);
	  rand=elevencent::ntoh<uint64_t>(rand);
	  date=elevencent::ntoh<datetime_t>(date);
	}
      };
      uint16_t cryptlen=0;
      RespLogin():RespCode(RESP_LOGIN){}
      void hton(){
	RespCode::hton();
	cryptlen=elevencent::hton<uint16_t>(cryptlen);
      }
    };
    class RespPubkey{
    public:
      uint16_t len;
      void ntoh(){
	len=elevencent::ntoh<uint16_t>(len);
      }
    };
    class RsaPubkey:public Head{
    public:
      uint16_t len;
      RsaPubkey(bool ton=true){
	len=g_pubkey.size();
	type=RESP_PUBKEY;	
	if(ton)
	  hton();
      }
      void hton(){
	Head::hton();
	len=htons(len);
      }
      void ntoh(){
	Head::ntoh();
	len=ntohs(len);
      }
    };
    class SignUp{
    public:
      resource_id_t uId;
      resource_mask_t uNameMask;
      resource_mask_t uPwdMask;
      resource_mask_t pwdMask;            
      uint16_t nameLen;
      uint16_t crypwdLen;      
      void ntoh(){
	uId=elevencent::ntoh<resource_id_t>(uId);
	uNameMask=elevencent::ntoh<resource_mask_t>(uNameMask);
	uPwdMask=elevencent::ntoh<resource_mask_t>(uPwdMask);
        pwdMask=elevencent::ntoh<resource_mask_t>(pwdMask);
	nameLen=elevencent::ntoh<uint16_t>(nameLen);
        crypwdLen=elevencent::ntoh<uint16_t>(crypwdLen);
      }
    };
    class Login{
    public:
      resource_id_t uId;
      resource_mask_t uNameMask;
      resource_mask_t uPwdMask;
      resource_mask_t pwdMask;            
      uint16_t nameLen;
      uint16_t crypwdLen;      
      void ntoh(){
	uId=elevencent::ntoh<resource_id_t>(uId);
	uNameMask=elevencent::ntoh<resource_mask_t>(uNameMask);
	uPwdMask=elevencent::ntoh<resource_mask_t>(uPwdMask);
        pwdMask=elevencent::ntoh<resource_mask_t>(pwdMask);
	nameLen=elevencent::ntoh<uint16_t>(nameLen);
        crypwdLen=elevencent::ntoh<uint16_t>(crypwdLen);
      }
    };
    class UserSendMsg{
    public:
      enum MSG_TYPE:uint16_t{
	STRING,
      };
      uint32_t cryptlen;      
      uint16_t msgType;
      uint16_t nameLen;
      uint32_t msgLen;
      void ntoh(){
	cryptlen=elevencent::ntoh<uint32_t>(cryptlen);
	msgType=elevencent::ntoh<uint16_t>(msgType);
	msgLen=elevencent::ntoh<uint32_t>(msgLen);
	nameLen=elevencent::ntoh<uint16_t>(nameLen);	
      }
    };
    class ReqUserMsg{
    public:
      class CryContent{
      public:
	RespLogin::Token token;
	datetime_t from,to;
	uint16_t nameLen;
	void ntoh(){
	  token.ntoh();
	  from=elevencent::ntoh<datetime_t>(from);
	  to=elevencent::ntoh<datetime_t>(to);
	  nameLen=elevencent::ntoh<uint16_t>(nameLen);
	}
      };
      uint32_t crylen;
      void ntoh(){
	crylen=elevencent::ntoh<uint32_t>(crylen);
      }
    };
    class ReqMsgUsers{
    public:
      class CryContent{
      public:
	RespLogin::Token token;
	void ntoh(){
	  token.ntoh();
	}
      };
      uint32_t crylen;
      void ntoh(){
	crylen=elevencent::ntoh<uint32_t>(crylen);
      }
    };
    class RespUserMsg:public RespCode{
    public:
      RespUserMsg():RespCode(RESP_USER_MSG){}
    };
    class FragPacket:public Head{
    public:
      uint8_t end=0;
      uint32_t pack_id;
      uint32_t total=1;
      uint32_t seq=0;
      uint32_t len;
      static uint32_t getFreePackId(){
	static volatile std::atomic<uint32_t> _s_pack_id;
	return _s_pack_id++;
      }
      void ntoh(){
	Head::ntoh();
	pack_id=elevencent::ntoh<uint32_t>(pack_id);
	total=elevencent::ntoh<uint32_t>(total);
	seq=elevencent::ntoh<uint32_t>(seq);
	len=elevencent::ntoh<uint32_t>(len);	
      }
      void hton(){
	Head::hton();
	pack_id=elevencent::hton<uint32_t>(pack_id);
	total=elevencent::hton<uint32_t>(total);
	seq=elevencent::hton<uint32_t>(seq);
	len=elevencent::hton<uint32_t>(len);		
      }
    };
    class RspUserMsgFragPacket:public FragPacket{
    public:
      class Result{
      public:
	resource_mask_t umask;
	resource_mask_t pmask;	
	datetime_t update_time;
	size_t post_content_len;
	uint16_t name_len;
	void hton(){
	  umask=elevencent::hton<resource_mask_t>(umask);
	  pmask=elevencent::hton<resource_mask_t>(pmask);
	  update_time=elevencent::hton<datetime_t>(update_time);
	  post_content_len=elevencent::hton<size_t>(post_content_len);
	  name_len=elevencent::hton<uint16_t>(name_len);
	}
	void ntoh(){
	  umask=elevencent::ntoh<resource_mask_t>(umask);
	  pmask=elevencent::ntoh<resource_mask_t>(pmask);
	  update_time=elevencent::ntoh<datetime_t>(update_time);
	  post_content_len=elevencent::ntoh<size_t>(post_content_len);
	  name_len=elevencent::ntoh<uint16_t>(name_len);	  
	}
      };
    };
    class RspMsgUsersFragPacket:public FragPacket{
    public:
    };    
  };
#pragma pack()    
}


#endif
