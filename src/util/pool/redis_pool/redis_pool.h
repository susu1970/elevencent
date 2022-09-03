#ifndef _ELEVENCENT_REDIS_POOL_H_
#define _ELEVENCENT_REDIS_POOL_H_

#include<hiredis/hiredis.h>
#include<unordered_map>
#include<string>

namespace elevencent{
  #define REDIS_POOL_HOST_NAME_KEY "hostname"
  #define REDIS_POOL_PORT_KEY "port"
  #define REDIS_POOL_CACHE_NUM_KEY "cachenum"      
  #define REDIS_POOL_MAX_NUM_KEY "maxnum"

  class RedisPool{
    std::unordered_map<redisContext*,bool>m_idleUMap,m_busyUMap;
    std::unordered_map<std::string,std::string>m_properties;
    short m_cacheNum,m_maxNum;
  private:
    redisContext*getContext();
    void recycleContext(redisContext*);
  public:
    RedisPool(std::unordered_map<std::string,std::string>properties);
    static redisContext*getRedisContext(RedisPool*pool);
    static void recycleRedisContext(RedisPool*pool,redisContext*);
    ~RedisPool();
  };
}

#endif
