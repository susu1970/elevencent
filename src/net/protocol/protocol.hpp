#ifndef ELEVENCENT_PROTOCOL_H_
#define ELEVENCENT_PROTOCOL_H_

#include<stdint.h>
#include"db_mapper.hpp"
#include"resource.4circle.h"

namespace elevencent{
#pragma pack(1)  
  class TcpProtocolS{
  public:
    class Head{
    public:
      uint16_t version:4;
      uint16_t type:12;
      enum TYPE:uint16_t{
	INSERT_USER_RESOURCE,
	INSERT_PASSWD_RESOURCE,
	USER_REF_RESOURCE,
	REQUEST_PUBKEY,
      };
      Head(const void*netbuf){
	uint16_t u16=ntohs(*((uint16_t*)netbuf));
	version=u16>>8;
	type=(u16<<4)>>8;
      }
    }; 
    class InsertUserResource{
    public:
      resource_id_t uId;
      resource_mask_t uResMask;
    };
    class InsertPasswdResource{
    public:
      resource_mask_t pwdResMask;
      uint16_t pwdLen;      
      char pwd[0];
    };
    class UserRefResource{
    public:
      resource_id_t uId;
      resource_id_t resId;
      resource_mask_t uResMask;
    };
  };
#pragma pack()    
  
}


#endif
