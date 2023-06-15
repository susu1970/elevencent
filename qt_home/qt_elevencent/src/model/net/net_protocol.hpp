#ifndef NET_PROTOCOL_HPP_
#define NET_PROTOCOL_HPP_

#include"global.h"
#include<iostream>
#include<unordered_map>

namespace qt_elevencent{
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
      Head(uint16_t type,uint8_t version=0b01):type(type),version(version){}
      Head(){}
      void h2n(){
	type=h2ns(type);
      }
      void n2h(){
	type=n2hs(type);
      }
    };
    class SignUp:public Head{
    public:
      resource_id_t uId;
      resource_mask_t uNameMask;
      resource_mask_t uPwdMask;
      resource_mask_t pwdMask;            
      uint16_t nameLen;
      uint16_t crypwdLen;      
      char priv[0];//name,crypwd
      void h2n(){
	Head::h2n();
	uId=qToBigEndian<resource_id_t>(uId);
	uNameMask=qToBigEndian<resource_mask_t>(uNameMask);
	uPwdMask=qToBigEndian<resource_mask_t>(uPwdMask);
	pwdMask=qToBigEndian<resource_mask_t>(pwdMask);
	nameLen=qToBigEndian<uint16_t>(nameLen);
        crypwdLen=qToBigEndian<uint16_t>(crypwdLen);	
      }
    };
    class Login:public Head{
    public:
      resource_id_t uId;
      resource_mask_t uNameMask;
      resource_mask_t uPwdMask;
      resource_mask_t pwdMask;            
      uint16_t nameLen;
      uint16_t crypwdLen;      
      char priv[0];//name,crypwd
      void h2n(){
	Head::h2n();
	uId=qToBigEndian<resource_id_t>(uId);
	uNameMask=qToBigEndian<resource_mask_t>(uNameMask);
	uPwdMask=qToBigEndian<resource_mask_t>(uPwdMask);
	pwdMask=qToBigEndian<resource_mask_t>(pwdMask);
	nameLen=qToBigEndian<uint16_t>(nameLen);
        crypwdLen=qToBigEndian<uint16_t>(crypwdLen);	
      }
    };    
    class RespCode{
    public:
      uint32_t code;
      void ntoh(){
	code=qFromBigEndian<uint32_t>(code);
      }
      bool isErr(){
	for(auto i=ERR_START<<1;i!=ERR_END;i<<=1)
	  if(i&code)
	    return true;
	return false;
      }
      bool isWarn(){
	for(auto i=WARN_START<<1;i!=WARN_END;i<<=1)
	  if(i&code)
	    return true;
	return false;
      }
      bool isOk(){
	return code==OK;
      }
    };
    class RespSignup:public RespCode{
    public:
      resource_id_t uId;
      void ntoh(){
	RespCode::ntoh();
	uId=qFromBigEndian<resource_id_t>(uId);
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
	Token&operator=(const Token&r){
	  rand=r.rand;
	  date=r.date;
	  uId=r.uId;
	  return *this;
	}
	Token(const Token&r){
	  *this=r;
	}
	bool operator==(const Token&r){
	  return rand==r.rand&&date==r.date&&uId==r.uId;
	}      
	void ntoh(){
	  uId=qFromBigEndian<resource_id_t>(uId);	
	  rand=qFromBigEndian<uint64_t>(rand);
	  date=qFromBigEndian<datetime_t>(date);
	}
	void hton(){
	  uId=qToBigEndian<resource_id_t>(uId);	
	  rand=qToBigEndian<uint64_t>(rand);
	  date=qToBigEndian<datetime_t>(date);
	}	
      };
      uint16_t cryptlen=0;
      void ntoh(){
	RespCode::ntoh();
	cryptlen=qFromBigEndian<uint16_t>(cryptlen);
      }
    };
    class RespUserSendMsg:public RespCode{
    public:
    };
    class RespUserReqMsg:public RespCode{
    public:
    };
    class RespMsgUsersCode:public RespCode{
    public:
    };
    class RespRsaPubkey{
    public:
      uint16_t len;
      void n2h(){
	len=n2hs(len);
      }
    };
    class RsaPubkey:public Head{
    public:
      uint16_t len;
      RsaPubkey():Head(RESP_PUBKEY){}
      void h2n(){
	Head::h2n();
	len=qToBigEndian<uint16_t>(len);
      }
    };
    class UserSendMsg:public Head{
    public:
      enum MSG_TYPE:uint16_t{
	STRING,
      };
      uint32_t cryptlen=0;      
      uint16_t msgType;
      uint16_t nameLen;
      uint32_t msgLen;
      UserSendMsg():Head(USER_SEND_MSG){}
      void h2n(){
	Head::h2n();
	cryptlen=qToBigEndian<uint32_t>(cryptlen);
        msgType=qToBigEndian<uint16_t>(msgType);
        nameLen=qToBigEndian<uint16_t>(nameLen);
        msgLen=qToBigEndian<uint32_t>(msgLen);		
      }
    };
    class UserReqMsg:public Head{
    public:
      uint32_t cryptlen;
      class CryContent{
      public:
	RespLogin::Token token;
	datetime_t from,to;
	uint16_t nameLen;
	void hton(){
	  token.hton();
	  from=qToBigEndian<datetime_t>(from);
	  to=qToBigEndian<datetime_t>(to);
	  nameLen=qToBigEndian<uint16_t>(nameLen);
	}
      };
      UserReqMsg():Head(USER_REQ_MSG){}
      void h2n(){
	Head::h2n();
	cryptlen=qToBigEndian<uint32_t>(cryptlen);	
      }
    };
    class ReqMsgUsers:public Head{
    public:
      uint32_t cryptlen;
      ReqMsgUsers():Head(REQ_MSG_USERS){}
      void h2n(){
	Head::h2n();
	cryptlen=qToBigEndian<uint32_t>(cryptlen);
      }
    };
    class FragPacket{
    public:
      uint8_t end=0;
      uint32_t pack_id;
      uint32_t total=1;
      uint32_t seq=0;
      uint32_t len;
      void ntoh(){
	pack_id=qFromBigEndian<uint32_t>(pack_id);
	total=qFromBigEndian<uint32_t>(total);
	seq=qFromBigEndian<uint32_t>(seq);
	len=qFromBigEndian<uint32_t>(len);	
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
	void ntoh(){
	  umask=qFromBigEndian<resource_mask_t>(umask);
	  pmask=qFromBigEndian<resource_mask_t>(pmask);
	  update_time=qFromBigEndian<datetime_t>(update_time);
	  post_content_len=qFromBigEndian<size_t>(post_content_len);
	  name_len=qFromBigEndian<uint16_t>(name_len);	  
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
