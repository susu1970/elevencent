#ifndef ELEVENCENT_RESOURCE_H_
#define ELEVENCENT_RESOURCE_H_

#include<unordered_map>
#include"db_mapper.hpp"
#include"resource.4circle.h"
#include"db_memory_cache.4circle.h"

namespace elevencent{
  enum class DB_PASSWD_RESOURCE_MASK:resource_mask_t{
    PLAIN=1<<1,
    MD5=1<<2,
    DFT=PLAIN,    

  };
  enum class DB_USER_RESOURCE_MASK:resource_mask_t{
    DFT
  };
  enum class DB_RESOURCE_MASK:resource_mask_t{
    //[2,32]resource type,[33,],[60,64]reserved
    USER_RESOURCE=1,
    NAME_RESOURCE=1<<1,
    FILE_RESOURCE=1<<2,
    PASSWD_RESOURCE=1<<3,
    POST_RESOURCE=1<<4,
    POST_CONTENT_RESOURCE=1<<5,

    DIRECT_DELETE=1<<62,
    AUTO_DELETE_REF0=1<<63,
  };
}

#endif
