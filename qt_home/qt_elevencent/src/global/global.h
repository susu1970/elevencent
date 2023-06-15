#ifndef GLOBAL_H_
#define GLOBAL_H_
#include<QScreen>
#include<QGuiApplication>
#include<iostream>
#include<QDebug>
#include<QtEndian>
#include<string>
namespace qt_elevencent{
#define SCREEN_RECT QGuiApplication::primaryScreen()->geometry()
#define SCREEN_W SCREEN_RECT.width()
#define SCREEN_H SCREEN_RECT.height()
#define MAX(a,b) ((a)>=(b)?(a):(b))
#define MIN(a,b) ((a)<=(b)?(a):(b))
  typedef unsigned char uint8_t;
  typedef unsigned short uint16_t;
  typedef unsigned int uint32_t;
  typedef unsigned long uint64_t;
  typedef long int64_t;
  typedef int int32_t;
  typedef short int16_t;
  typedef char int8_t;
  typedef unsigned int resource_type_t;
  typedef unsigned int resource_id_t;
  typedef unsigned long resource_mask_t;
  typedef unsigned long datetime_t;  
  
  static inline bool is_little_endian(){
    const static uint32_t a=1;
    return (*(char*)&a)==1;
  }
  static inline uint16_t n2hs(uint16_t u16){
    return qFromBigEndian<uint16_t>(u16);
  }
  static inline uint16_t h2ns(uint16_t u16){
    return qToBigEndian<uint16_t>(u16);
  }
  static inline uint32_t n2hl(uint32_t u32){
    return qFromBigEndian<uint32_t>(u32);
  }
  static inline uint32_t h2nl(uint32_t u32){
    return qToBigEndian<uint16_t>(u32);
  }
  extern std::string g_privkey,g_pubkey;
}

#endif
