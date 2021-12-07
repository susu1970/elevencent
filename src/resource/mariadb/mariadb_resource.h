#ifndef _MARIADB_RESOURCE_H_
#define _MARIADB_RESOURCE_H_

#include<string>
#include<mariadb/conncpp.hpp>
#include"db_mapper.hpp"
#include"db_memory_cache.h"
#include"mariadb_pool.h"

namespace elevencent{
#define MAX_MARIADB_PACKET 1073741824//more details on: https://mariadb.com/kb/en/server-system-variables/#max_allowed_packet.Moreover,we limit that each packet send to mariadb has MAX_ALLOWED_PACKET bytes at most.

  enum class DB_CACHE_TYPE:unsigned char{
    AROUND,
    BACK,
    THROUGH,
    DFT=DB_CACHE_TYPE::THROUGH
  };
  //non-thread-safety
  class MariadbResource{
    MariadbPool m_dbPool;
    DbMemoryCache m_dbMemCache;
    unsigned char m_resourceIdBitMap[DB_MAX_RESOURCE_ID_NUM/8+1];
    resource_id_t m_resourceIdFreeStart;
    sql::Connection*m_conn;
  private:
    sql::Connection*getConn();
    void setResourceIdBitMap(resource_id_t resourceId);
    void unSetResourceIdBitMap(resource_id_t resourceId);
  public:
    bool isSetResourceIdBitMap(resource_id_t resourceId);
    bool consumeFreeResourceId(resource_id_t*resourceId);
    bool peekFreeResourceId(resource_id_t*resourceId);    
    void MariadbResource::insertPasswdResource(resource_id_t*passwdResourceId,std::string passwd,resource_mask_t passwdResourceMask=DB_PASSWD_RESOURCE_MASK::DFT,resource_mask_t resourceMask=DB_RESOURCE_MASK::PASSWD_RESOURCE|DB_RESOURCE_MASK::AUTO_DELETE_REF0,DB_CACHE_TYPE type=DB_MEMORY_CACHE_TYPE::DFT);
    bool insertUserResource(resource_id_t userResourceId,resource_mask_t userResourceMask,resource_mask_t resourceMask=DB_RESOURCE_MASK::USER_RESOURCE|DB_RESOURCE_MASK::DIRECT_DELETE,DB_CACHE_TYPE type=DB_MEMORY_CACHE_TYPE::DFT);
    bool insertUserResource(resource_id_t*userResourceId,resource_mask_t userResourceMask,resource_mask_t resourceMask=DB_RESOURCE_MASK::USER_RESOURCE|DB_RESOURCE_MASK::DIRECT_DELETE,DB_CACHE_TYPE type=DB_MEMORY_CACHE_TYPE::DFT);
    bool deleteUserResource(resource_id_t userResourceId);
  };
}


#endif /* _MARIADB_RESOURCE_H_ */
