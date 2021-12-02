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
  private:
    sql::Connection*getConn();

  public:
    void insertResource(resource_id_t resourceId,resource_mask_t resource_mask,resource_ref_t resourceRef)
    void setResourceIdBitMap(resource_id_t resourceId);
    void unSetResourceIdBitMap(resource_id_t resourceId);
    bool isSetResourceIdBitMap(resource_id_t resourceId);
    void insertPasswdResource(resource_id_t resourceId,resource_mask_t resourceMask,std::string passwd);
    bool increResourceRef(resource_id_t userResourceId);
    bool createUserResource(resource_id_t userResourceId,resource_mask_t userMask,std::string passwd,resource_mask_t passwdMask,resource_mask_t userResourceMask,resource_mask_t passwdResourceMask,DB_CACHE_TYPE type=DB_CACHE_TYPE::DB_CACHE_DFT);
    resource_id_t consumeFreeResourceId();
  };
}


#endif /* _MARIADB_RESOURCE_H_ */
