#include"redis_pool.h"
#include"global.h"

using namespace std;
using namespace elevencent;

static short g_cacheNumDft=g_processNum;
static short g_maxNumDft=MAX(g_processNum,g_cacheNumDft);

#define REDIS_POOL_HOST_NAME_DFT "localhost"
#define REDIS_POOL_PORT_DFT "6379"

RedisPool::RedisPool(unordered_map<string,string>properties){
  m_cacheNum=g_cacheNumDft;
  m_maxNum=g_maxNumDft;
  for(auto iter=properties.begin();iter!=properties.end();++iter)
    m_properties[iter->first]=iter->second;
  if(m_properties.find(REDIS_POOL_HOST_NAME_KEY)==m_properties.end())
    m_properties[REDIS_POOL_HOST_NAME_KEY]=REDIS_POOL_HOST_NAME_DFT;
  if(m_properties.find(REDIS_POOL_PORT_KEY)==m_properties.end())
    m_properties[REDIS_POOL_PORT_KEY]=REDIS_POOL_PORT_DFT;
  if(m_properties.find(REDIS_POOL_CACHE_NUM_KEY)!=m_properties.end())
    m_cacheNum=stoi(m_properties[REDIS_POOL_CACHE_NUM_KEY]);
  if(m_properties.find(REDIS_POOL_MAX_NUM_KEY)!=m_properties.end())
    m_maxNum=stoi(m_properties[REDIS_POOL_MAX_NUM_KEY]);
  if(m_cacheNum<=0)
    m_cacheNum=g_cacheNumDft;
  if(m_maxNum<=0)
    m_maxNum=m_cacheNum;
}
redisContext*RedisPool::getContext(){
  redisContext*ret;      
  if(!m_idleUMap.empty()){
    ret=m_idleUMap.begin()->first;
    m_idleUMap.erase(ret);
    m_busyUMap[ret]=true;
    return ret;
  }
  short curNum=m_busyUMap.size();
  short num=m_cacheNum-curNum;
  if(num<=0&&(num=m_maxNum-curNum)>1)
    num=1;
  if(num<=0)return 0;
  if(!(ret=redisConnectWithTimeout(m_properties[REDIS_POOL_HOST_NAME_KEY].c_str(),stoi(m_properties[REDIS_POOL_PORT_KEY]),{0,0}))||ret->err){
    DEBUG_MSG("redisConnectWithTimeout error");
    return 0;	
  }
  m_busyUMap[ret]=true;
  redisContext*tmpconn;
  while(--num>0){
    if(!(tmpconn=redisConnectWithTimeout(m_properties[REDIS_POOL_HOST_NAME_KEY].c_str(),stoi(m_properties[REDIS_POOL_PORT_KEY]),{0,0}))||tmpconn->err){
      DEBUG_MSG("redisConnectWithTimeout error");
      return ret;    
    }
    m_idleUMap[tmpconn]=true;
  }
  return ret;		
}

void RedisPool::recycleContext(redisContext*conn){
  if(m_busyUMap.find(conn)==m_busyUMap.end())
    return;
  if(m_idleUMap.empty()){
    m_busyUMap.erase(conn);
    m_idleUMap[conn]=true;
    return;
  }
  short curNum=m_busyUMap.size()+m_idleUMap.size();
  short cacheD=m_cacheNum-curNum;
  if(cacheD<0){
    m_busyUMap.erase(conn);
    redisFree(conn);
    return;
  }
  m_idleUMap[conn]=true;
  m_busyUMap.erase(conn);  
  return;
}

redisContext*RedisPool::getRedisContext(RedisPool*pool){
  return pool?pool->getContext():0;
}
void RedisPool::recycleRedisContext(RedisPool*pool,redisContext*conn){
  if(!conn)return;
  if(!pool){
    redisFree(conn);
    return;
  }
  pool->recycleContext(conn);
}

RedisPool::~RedisPool(){
  for(auto iter=m_idleUMap.begin();iter!=m_idleUMap.end();++iter){
    if(!iter->first)continue;
    redisFree((redisContext*)iter->first);
  }
}
