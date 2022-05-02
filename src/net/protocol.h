#ifndef ELEVENCENT_PROTOCOL_H_
#define ELEVENCENT_PROTOCOL_H_

#include<stdint.h>

namespace elevencent{
  enum class PROTOCOL_TYPE:uint8_t{
    REQUEST_LOGIN,
    
  };
#pragma pack(1)  
  class Protocol{
  public:
    uint8_t version;    
    uint16_t len;
    uint8_t type;
    char data[0];
  public:
    
  };
#pragma pack()    
  
}


#endif
