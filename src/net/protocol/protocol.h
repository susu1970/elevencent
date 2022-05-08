#ifndef ELEVENCENT_PROTOCOL_H_
#define ELEVENCENT_PROTOCOL_H_

#include<stdint.h>
#include"db_mapper.hpp"
#include"resource.4circle.h"

namespace elevencent{
#pragma pack(1)  
  class TcpProtocolC{
  public:
    class Head{
    public:
      uint16_t version:4;
      uint16_t type:12;
      enum class TYPE:uint16_t{
	INSERT_USER_RESOURCE=1,
	INSERT_PASSWD_RESOURCE,
	USER_REF_RESOURCE,

	REQUEST_PUBKEY,
      };      
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
  class TcpProtocolS{
  public:
    class Head{
    public:
      uint16_t version:4;
      uint16_t type:12;
      enum class TYPE:uint16_t{
	RET_CODE=1,
	RESP_PUBKEY,
	
	
	UNKNOWN,
      };      
    };
    enum class RETCODE:uint8_t{
      INSERT_USER_RESOURCE_OK,
      INSERT_USER_RESOURCE_FAIL,

      INSERT_PASSWD_RESOURCE_OK,
      INSERT_PASSWD_RESOURCE_FAIL,
      
    };
  };
#pragma pack()    
  
}


#endif
