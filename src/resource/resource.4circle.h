#ifndef RESOURCE_4CIRCLE_H_
#define RESOURCE_4CIRCLE_H_

namespace elevencent{
#define MAX_RESOURCE_NAME MIN(255,MAX_ALLOWED_PACKET)
#define MAX_RESOURCE_FILE_NAME MIN(1026,MAX_ALLOWED_PACKET)
#define MAX_RESOURCE_FILE_LOCATION MIN(10240,MAX_ALLOWED_PACKET)
#define MAX_RESOURCE_PASSWD MIN(4096,MAX_ALLOWED_PACKET)
#define MAX_RESOURCE_POST_CONTENT MIN(4294967296,MAX_ALLOWED_PACKET)
  typedef unsigned int resource_type_t;
  enum class RESOURCE_TYPE:resource_type_t{
    START=1,
    ALL=(resource_type_t)-1,
    NAME_RESOURCE=1<<1,
    FILE_RESOURCE=1<<2,
    PASSWD_RESOURCE=1<<3,
    POST_RESOURCE=1<<4,
    POST_CONTENT_RESOURCE=1<<5,
    RESOURCE=1<<6,
    USER_RESOURCE=1<<7,
    END=1<<8
  };

  inline resource_type_t operator|(RESOURCE_TYPE a,RESOURCE_TYPE b){
    return ((resource_type_t)a)|((resource_type_t)b);
  }
  inline RESOURCE_TYPE operator|(resource_type_t a,RESOURCE_TYPE b){
    return (RESOURCE_TYPE)(((resource_type_t)a)|((resource_type_t)b));
  }
  inline RESOURCE_TYPE operator|(RESOURCE_TYPE a,resource_type_t b){
    return (RESOURCE_TYPE)(((resource_type_t)a)|((resource_type_t)b));
  }      
  inline resource_type_t operator&(RESOURCE_TYPE a,RESOURCE_TYPE b){
    return ((resource_type_t)a)&((resource_type_t)b);
  }
  inline RESOURCE_TYPE operator&(resource_type_t a,RESOURCE_TYPE b){
    return (RESOURCE_TYPE)(((resource_type_t)a)&((resource_type_t)b));
  }
  inline RESOURCE_TYPE operator&(RESOURCE_TYPE a,resource_type_t b){
    return (RESOURCE_TYPE)(((resource_type_t)a)&((resource_type_t)b));
  }        
  inline bool operator!(RESOURCE_TYPE a){
    return !((resource_type_t)a);
  }
  inline RESOURCE_TYPE operator|=(RESOURCE_TYPE&a,RESOURCE_TYPE b){
    return a=(RESOURCE_TYPE)(((resource_type_t)a)|((resource_type_t)b));
  }
  inline RESOURCE_TYPE operator&=(RESOURCE_TYPE&a,RESOURCE_TYPE b){
    return a=(RESOURCE_TYPE)(((resource_type_t)a)&((resource_type_t)b));
  }  
  inline RESOURCE_TYPE operator<<(RESOURCE_TYPE a,unsigned short b){
    return (RESOURCE_TYPE)((resource_type_t)a<<b);
  }  
  inline RESOURCE_TYPE operator<<=(RESOURCE_TYPE&a,unsigned short b){
    return a=a<<b;
  }  
  inline RESOURCE_TYPE operator>>(RESOURCE_TYPE a,unsigned short b){
    return (RESOURCE_TYPE)((resource_type_t)a>>b);
  }  
  inline RESOURCE_TYPE operator>>=(RESOURCE_TYPE&a,unsigned short b){
    return a=a>>b;
  }  
  class Resource;  
}

#endif
