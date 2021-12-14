#ifndef _MARIADB_RESOURCE_H_
#define _MARIADB_RESOURCE_H_

#include<string>
#include<unordered_map>
#include<mariadb/conncpp.hpp>
#include<INIReader.h>
#include"db_mapper.hpp"
#include"db_memory_cache.h"
#include"mariadb_pool.h"
#include"resource.h"

namespace elevencent{
#define MAX_MARIADB_PACKET 1073741824
  enum class DB_CACHE_TYPE:unsigned char{
    AROUND,
    BACK,
    THROUGH,
    DFT=DB_CACHE_TYPE::THROUGH
  };
  #define MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_LIMIT_SIZE "memory_cache_limit_size"
  #define MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_REPLACEMENT "memory_cache_replacement"
  #define MARIADB_RESOURCE_POOL_INI_SECTION_POOL "pool"
  #define MARIADB_RESOURCE_KEY_RESOURCE "resource"
  #define MARIADB_RESOURCE_KEY_USER_RESOURCE "user_resource"
  #define MARIADB_RESOURCE_KEY_NAME_RESOURCE "name_resource"
  #define MARIADB_RESOURCE_KEY_FILE_RESOURCE "file_resource"
  #define MARIADB_RESOURCE_KEY_PASSWD_RESOURCE "passwd_resource"
  #define MARIADB_RESOURCE_KEY_POST_RESOURCE "post_resource"
  #define MARIADB_RESOURCE_KEY_POST_CONTENT_RESOURCE "post_content_resource"
  #define MARIADB_RESOURCE_KEY_REPLACEMENT_RR "rr"  
  

  //non-thread-safety
  class MariadbResource{
    MariadbPool*m_dbPool;
    DbMemoryCache*m_dbMemCache;
    unsigned char m_resourceIdBitMap[DB_MAX_RESOURCE_ID_NUM/8+1];
    resource_id_t m_resourceIdFreeStart;
    sql::Connection*m_conn;
    size_t m_max_allowed_packet;//more details on: https://mariadb.com/kb/en/server-system-variables/#max_allowed_packet
  private:
    sql::Connection*getConn();    
    sql::Connection*getConn(bool autoCommit);
    void setResourceIdBitMap(resource_id_t resourceId);
    void unSetResourceIdBitMap(resource_id_t resourceId);
    std::unordered_map<std::string,DB_MEMORY_CACHE_REPLACEMENT>m_str2replacementUmap{
      {"rr",DB_MEMORY_CACHE_REPLACEMENT::RR},
      {"lru",DB_MEMORY_CACHE_REPLACEMENT::LRU},
      {"fifo",DB_MEMORY_CACHE_REPLACEMENT::FIFO},
      {"dft",DB_MEMORY_CACHE_REPLACEMENT::DFT}
    };
    bool consumeFreeResourceId(resource_id_t*resourceId);
  public:
    bool insertPasswdResource(resource_id_t*passwdResourceId,std::string passwd,resource_mask_t passwdResourceMask=(resource_mask_t)DB_PASSWD_RESOURCE_MASK::DFT,resource_mask_t resourceMask=(resource_mask_t)(DB_RESOURCE_MASK::PASSWD_RESOURCE|DB_RESOURCE_MASK::AUTO_DELETE_REF0),DB_MEMORY_CACHE_TYPE type=DB_MEMORY_CACHE_TYPE::DFT);        
    MariadbResource(std::string initFile);
    bool isSetResourceIdBitMap(resource_id_t resourceId);
    bool peekFreeResourceId(resource_id_t*resourceId);    
    bool insertUserResource(resource_id_t userResourceId,resource_mask_t userResourceMask,resource_mask_t resourceMask=(resource_mask_t)(DB_RESOURCE_MASK::USER_RESOURCE|DB_RESOURCE_MASK::DIRECT_DELETE),DB_MEMORY_CACHE_TYPE type=DB_MEMORY_CACHE_TYPE::DFT);
    bool insertUserResource(resource_id_t*userResourceId,resource_mask_t userResourceMask,resource_mask_t resourceMask=(resource_mask_t)(DB_RESOURCE_MASK::USER_RESOURCE|DB_RESOURCE_MASK::DIRECT_DELETE),DB_MEMORY_CACHE_TYPE type=DB_MEMORY_CACHE_TYPE::DFT);
    bool insertUserResource(resource_id_t userResourceId,resource_mask_t userResourceMask,std::string passwd,resource_mask_t passwdUserResourceMask,resource_mask_t resourceUserMask,resource_mask_t passwdResourceMask,resource_mask_t resourcePasswdMask,DB_MEMORY_CACHE_TYPE type);
    bool insertUserResource(resource_id_t*userResourceId,resource_mask_t userResourceMask,std::string passwd,resource_mask_t passwdUserResourceMask,resource_mask_t resourceUserMask,resource_mask_t passwdResourceMask,resource_mask_t resourcePasswdMask,DB_MEMORY_CACHE_TYPE type);        
    ~MariadbResource();
  };
}


#endif /* _MARIADB_RESOURCE_H_ */
