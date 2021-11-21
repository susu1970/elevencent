#ifndef _MARIADB_RESOURCE_H_
#define _MARIADB_RESOURCE_H_

#include<string>
#include<mariadb/conncpp.hpp>
#include"db_mapper.hpp"
#include"db_memory_cache.h"
#include"mariadb_pool.h"

namespace elevencent{
#define MAX_MARIADB_PACKET 1073741824//more details on: https://mariadb.com/kb/en/server-system-variables/#max_allowed_packet.Moreover,we limit that each packet send to mariadb has MAX_ALLOWED_PACKET bytes at most.
  enum class DB_CACHE_TYPE:unsigned short{
    START=1,

    AROUND=1<<1,
    BACK=1<<2,
    THROUGH=1<<3,
    FLUSH=1<<4,

    END=1<<5
  };
  //non-thread-safety
  class MariadbResource{
    MariadbPool m_dbPool;
    DbMemoryCache m_dbMemCache;
  public:
    bool isExistResourceId(resource_id_t resourceId,DB_CACHE_TYPE type);
    bool createUserResource(resource_id_t userResourceId,resource_mask_t userMask,std::string passwd,resource_mask_t passwdMask,resource_mask_t userResourceMask,resource_mask_t passwdResourceMask);
    
  };
}


#endif /* _MARIADB_RESOURCE_H_ */
