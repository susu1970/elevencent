#ifndef ELEVENCENT_GLOBAL_H_
#define ELEVENCENT_GLOBAL_H_

#include<cassert>
#include<sys/time.h>
#include<iostream>
#include<string>
#include<stdio.h>
#include<syslog.h>
#include<unistd.h>
#include<stdint.h>
#include<arpa/inet.h>

namespace elevencent{
  
#define BETA 1
#define DEBUG 1
  
#define MAX(a,b) ((a)>=(b)?(a):(b))
#define MIN(a,b) ((a)<=(b)?(a):(b))
#define CUR_TIME_CSTR ({time_t now=time(0);char buf[80];		\
      strftime(buf,sizeof(buf),"%b %e %Y.%X",localtime(&now));buf;})
#define LOCATE_INFO_STR (std::string("compile time: ")+std::string(__DATE__)+"."+std::string(__TIME__)+"\nrunning time: "+std::string(CUR_TIME_CSTR)+"\nrunning timestamp: "+std::to_string(time(0))+"\nfile: "+std::string(__FILE__)+"\nline: "+std::to_string(__LINE__)+"\nfunc: "+__PRETTY_FUNCTION__)
#define LOCATE_PRETTY_INFO_STR (std::string("running timestamp: ")+std::to_string(time(0))+",file: "+std::string(__FILE__)+",line: "+std::to_string(__LINE__))
#define DEBUG_MSG(msg) do{\
    if(!DEBUG)break;							\
    std::cout<<"\n------<DEBUG MSG>-----\n"<<LOCATE_INFO_STR<<"\n------<msg>------\n"<<std::string("")+msg<<"\n------</msg>------\n------</DEBUG MSG>-----\n";}while(0)
#define DEBUG_PRETTY_MSG(msg) do{\
    if(!DEBUG)break;\
    std::cout<<LOCATE_PRETTY_INFO_STR<<", msg: "<<msg<<"\n";}while(0)
#define DEBUG_ASSERT(cond,msg) do{\
  if(!DEBUG||cond)break;						\
  std::cout<<"\n------<DEBUG ASSERT>-----\n"<<LOCATE_INFO_STR<<"\n"<<"msg: "<<msg<<"\n------</DEBUG ASSERT>-----\n"; \
    assert(0);}while(0)
#define DEBUG_PRETTY_ASSERT(cond,msg) do{\
    if(!DEBUG||cond)break;\
    std::cout<<"assert failed, "<<LOCATE_PRETTY_INFO_STR<<", msg: "<<std::string("")+msg<<"\n";\
    assert(0);}while(0)
#define SYSLOG(priority,format,...) do{\
    syslog(priority,std::string(LOCATE_INFO_STR+"\n"+format).c_str(),##__VA_ARGS__);}while(0)
#define ASSERT(cond,msg) do{\
    if(cond)break;\
    SYSLOG(LOG_ERR,"assert failed: %s",msg);\
    assert(0);}while(0)
#define likely(x) __builtin_expect(!!(x),1)
#define unlikely(x) __builtin_expect(!!(x),0)
  static int8_t g_processNum=MAX(sysconf(_SC_NPROCESSORS_CONF),1);

#define BITMAP8_SET(bitmap,pos) bitmap[pos/8]|=(((uint8_t)1)<<(pos%8))
#define BITMAP8_UNSET(bitmap,pos) bitmap[pos/8]&=(~(((uint8_t)1)<<(pos%8)))
#define BITMAP8_ISSET(bitmap,pos) (bitmap[pos/8]&(((uint8_t)1)<<(pos%8)))
#define CAS_BOOL(ptr,oldval,newval) __sync_bool_compare_and_swap(ptr,oldval,newval)
#define CSA_VAL(ptr,oldval,newval) __sync_val_compare_and_swap(ptr,oldval,newval)
#define offset_of(type,mem) ((size_t)&((type*)0)->mem)
#define container_of(ptr,type,mem) ((type*)((char*)ptr-offset_of(type,mem)))
  static inline bool is_little_endian(){
    static uint16_t a=1;
    return *((uint8_t*)&a)==1;
  }
  static inline uint64_t ntoh64(uint64_t u64){
    if(!is_little_endian())
      return u64;
    *((uint32_t*)&u64)=ntohl(*((uint32_t*)&u64));
    *((uint32_t*)&u64+1)=ntohl(*((uint32_t*)&u64+1));
    (*((uint32_t*)&u64))^=(*((uint32_t*)&u64+1));
    (*((uint32_t*)&u64+1))^=(*((uint32_t*)&u64));
    (*((uint32_t*)&u64))^=(*((uint32_t*)&u64+1));    
    return u64;
  }
  static inline uint64_t hton64(uint64_t u64){
    if(!is_little_endian())
      return u64;
    *((uint32_t*)&u64)=htonl(*((uint32_t*)&u64));
    *((uint32_t*)&u64+1)=htonl(*((uint32_t*)&u64+1));
    (*((uint32_t*)&u64))^=(*((uint32_t*)&u64+1));
    (*((uint32_t*)&u64+1))^=(*((uint32_t*)&u64));
    (*((uint32_t*)&u64))^=(*((uint32_t*)&u64+1));    
    return u64;    
  }
  template <typename T>
  static T ntoh(T u){
    switch(sizeof(T)){
    case 2:
      return ntohs(u);
      break;
    case 4:
      return ntohl(u);
      break;
    case 8:
      return ntoh64(u);
      break;
    default:
      break;
    }
    return u;
  }
  template <typename T>
  static T hton(T u){
    switch(sizeof(T)){
    case 2:
      return htons(u);
      break;
    case 4:
      return htonl(u);
      break;
    case 8:
      return hton64(u);
      break;
    default:
      break;
    }
    return u;
  }
  static inline uint64_t getCurUsec(){
    struct timeval tv;
    gettimeofday(&tv,0);
    return tv.tv_usec+tv.tv_sec*1000000;
  }
  static inline uint64_t getCurMsec(){
    return getCurUsec()/1000;
  }
  extern std::string g_pubkey;
  extern std::string g_privkey;
}



#endif






