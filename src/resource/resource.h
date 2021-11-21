#ifndef ELEVENCENT_RESOURCE_H_
#define ELEVENCENT_RESOURCE_H_

#include<unordered_map>
#include"db_mapper.hpp"
#include"resource.4circle.h"
#include"db_memory_cache.4circle.h"

namespace elevencent{
  enum class DB_PASSWD_RESOURCE_MASK:resource_mask_t{
    DFT=0,
    PLAIN=1<<1,
    MD5=1<<2,
    
    ALL=(resource_mask_t)-1
  };
  enum class DB_USER_RESOURCE_MASK:resource_mask_t{
    DFT=0,

    ALL=(resource_mask_t)-1
  }
}

#endif
