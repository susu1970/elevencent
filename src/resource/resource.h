#ifndef ELEVENCENT_RESOURCE_H_
#define ELEVENCENT_RESOURCE_H_

#include<unordered_map>
#include"db_mapper.hpp"
#include"resource.4circle.h"
//#include"db_memory_cache.4circle.h"

namespace elevencent{
  enum class DB_PASSWD_RESOURCE_MASK:resource_mask_t{
    PLAIN=(resource_mask_t)1<<1,
    MD5=(resource_mask_t)1<<2,
    DFT=PLAIN,    
  };
  inline resource_mask_t operator|(DB_PASSWD_RESOURCE_MASK a,DB_PASSWD_RESOURCE_MASK b){
    return (((resource_mask_t)a)|((resource_mask_t)b));
  }
  inline resource_mask_t operator|(resource_mask_t a,DB_PASSWD_RESOURCE_MASK b){
    return (((resource_mask_t)a)|((resource_mask_t)b));
  }
  inline resource_mask_t operator|(DB_PASSWD_RESOURCE_MASK a,resource_mask_t b){
    return (((resource_mask_t)a)|((resource_mask_t)b));
  }      
  inline resource_mask_t operator&(DB_PASSWD_RESOURCE_MASK a,DB_PASSWD_RESOURCE_MASK b){
    return (((resource_mask_t)a)&((resource_mask_t)b));
  }
  inline resource_mask_t operator&(resource_mask_t a,DB_PASSWD_RESOURCE_MASK b){
    return (((resource_mask_t)a)&((resource_mask_t)b));
  }
  inline resource_mask_t operator&(DB_PASSWD_RESOURCE_MASK a,resource_mask_t b){
    return (((resource_mask_t)a)&((resource_mask_t)b));
  }      
  inline resource_mask_t operator<<(DB_PASSWD_RESOURCE_MASK a,unsigned char b){
    return ((resource_mask_t)a<<b);
  }
  inline resource_mask_t operator<<=(DB_PASSWD_RESOURCE_MASK&a,unsigned char b){
    return (resource_mask_t)(a=(DB_PASSWD_RESOURCE_MASK)(a<<b));
  }  
  inline resource_mask_t operator>>(DB_PASSWD_RESOURCE_MASK a,unsigned char b){
    return ((resource_mask_t)a>>b);
  }  
  inline resource_mask_t operator>>=(DB_PASSWD_RESOURCE_MASK&a,unsigned char b){
    return (resource_mask_t)(a=(DB_PASSWD_RESOURCE_MASK)(a>>b));
  }  
  inline resource_mask_t operator|=(DB_PASSWD_RESOURCE_MASK&a,DB_PASSWD_RESOURCE_MASK b){
    return (resource_mask_t)(a=(DB_PASSWD_RESOURCE_MASK)(a|b));
  }
  inline resource_mask_t operator&=(DB_PASSWD_RESOURCE_MASK&a,DB_PASSWD_RESOURCE_MASK b){
    return (resource_mask_t)(a=(DB_PASSWD_RESOURCE_MASK)(a&b));
  }
  inline bool operator!(DB_PASSWD_RESOURCE_MASK a){
    return !((resource_mask_t)a);
  }
  enum class DB_USER_RESOURCE_MASK:resource_mask_t{
    DFT
  };
  enum class DB_RESOURCE_MASK:resource_mask_t{
    //[2,32]resource type,[33,],[60,64]reserved
    USER_RESOURCE=(resource_mask_t)1,
    NAME_RESOURCE=(resource_mask_t)1<<1,
    FILE_RESOURCE=(resource_mask_t)1<<2,
    PASSWD_RESOURCE=(resource_mask_t)1<<3,
    POST_RESOURCE=(resource_mask_t)1<<4,
    POST_CONTENT_RESOURCE=(resource_mask_t)1<<5,
    
    DIRECT_DELETE=(resource_mask_t)1<<62,
    AUTO_DELETE_REF0=(resource_mask_t)1<<63,
  };
  inline resource_mask_t operator|(DB_RESOURCE_MASK a,DB_RESOURCE_MASK b){
    return (((resource_mask_t)a)|((resource_mask_t)b));
  }
  inline resource_mask_t operator|(resource_mask_t a,DB_RESOURCE_MASK b){
    return (((resource_mask_t)a)|((resource_mask_t)b));
  }
  inline resource_mask_t operator|(DB_RESOURCE_MASK a,resource_mask_t b){
    return (((resource_mask_t)a)|((resource_mask_t)b));
  }      
  inline resource_mask_t operator&(DB_RESOURCE_MASK a,DB_RESOURCE_MASK b){
    return (((resource_mask_t)a)&((resource_mask_t)b));
  }
  inline resource_mask_t operator&(resource_mask_t a,DB_RESOURCE_MASK b){
    return (((resource_mask_t)a)&((resource_mask_t)b));
  }
  inline resource_mask_t operator&(DB_RESOURCE_MASK a,resource_mask_t b){
    return (((resource_mask_t)a)&((resource_mask_t)b));
  }      
  inline resource_mask_t operator<<(DB_RESOURCE_MASK a,unsigned char b){
    return ((resource_mask_t)a<<b);
  }
  inline resource_mask_t operator<<=(DB_RESOURCE_MASK&a,unsigned char b){
    return (resource_mask_t)(a=(DB_RESOURCE_MASK)(a<<b));
  }  
  inline resource_mask_t operator>>(DB_RESOURCE_MASK a,unsigned char b){
    return ((resource_mask_t)a>>b);
  }  
  inline resource_mask_t operator>>=(DB_RESOURCE_MASK&a,unsigned char b){
    return (resource_mask_t)(a=(DB_RESOURCE_MASK)(a>>b));
  }  
  inline resource_mask_t operator|=(DB_RESOURCE_MASK&a,DB_RESOURCE_MASK b){
    return (resource_mask_t)(a=(DB_RESOURCE_MASK)(a|b));
  }
  inline resource_mask_t operator&=(DB_RESOURCE_MASK&a,DB_RESOURCE_MASK b){
    return (resource_mask_t)(a=(DB_RESOURCE_MASK)(a&b));
  }
  inline bool operator!(DB_RESOURCE_MASK a){
    return !((resource_mask_t)a);
  }
}

#endif
