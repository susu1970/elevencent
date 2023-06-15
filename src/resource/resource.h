#ifndef ELEVENCENT_RESOURCE_H_
#define ELEVENCENT_RESOURCE_H_

#include<unordered_map>
#include<string>
#include<INIReader.h>
#include<pthread.h>
#include<cstring>
#include<pthread.h>

#include"db_mapper.hpp"
#include"db_mem_cache.h"
#include"resource_global.h"
#include"mariadb_pool.h"
#include"resource_limit.hpp"

namespace elevencent{
  class Resource{
  public:
    class IStreamBlobBuf:public std::streambuf{
    public:
      IStreamBlobBuf(char*d,size_t s){
	setg(d,d,d+s);
      }
    };
    enum ERRNO:uint64_t{
      OK=0,
      ERR_START=1,

      ERR_COMM=ERR_START<<1,
      ERR_RESOURCE_ID=ERR_START<<2,
      ERR_LIMIT_SIZE=ERR_START<<3,
      ERR_DB=ERR_START<<4,
      ERR_RESOURCE_TYPE=ERR_START<<5,
      ERR_RESOURCE_MASK=ERR_START<<6,      

      ERR_END=ERR_START<<15,

      WARN_START=ERR_END<<1,
      
      WARN_COMM=WARN_START<<1,
      WARN_RESOURCE_EXIST=WARN_START<<2,
      
      WARN_END=WARN_START<<20,
    };
  private:
    MariadbPool*m_dbPool;
    DbMemCache*m_dbMemCache;
    uint8_t m_resourceIdBitMap[RESOURCE_ID_RANGE_MAX/8+1];
    resource_id_t m_resourceIdFreeStart;
    size_t m_maxAllowedPacket;//more details on: https://mariadb.com/kb/en/server-system-variables/#max_allowed_packet
  private:
    void setResourceIdBitMap(resource_id_t resourceId);
    void unSetResourceIdBitMap(resource_id_t resourceId);
    bool consumeFreeResourceId(resource_id_t*resourceId);
    bool consumeResourceId(resource_id_t resourceId);    
  public:
    class ScopeMutex{
      Resource*r;
    public:
      ScopeMutex(Resource*r_):r(r_){
	pthread_mutex_lock(&r->m_mutex);
      }
      ~ScopeMutex(){
	pthread_mutex_unlock(&r->m_mutex);
      }
    };
    pthread_mutex_t m_mutex;
    sql::Connection**getConn(bool autoCommit);
    void putConn(sql::Connection**);
    Resource(std::string initFile);
    bool isSetResourceIdBitMap(resource_id_t resourceId);
    bool peekFreeResourceId(resource_id_t*resourceId);
    ~Resource();
    ERRNO selectNameResource(std::string name,resource_id_t*nId);
    ERRNO selectPasswdResource(std::string pwd,resource_id_t*pId,resource_mask_t passwdMask=RESOURCE_MASK_PASSWD_DFT);    
    ERRNO selectUserResourceByNameId(resource_id_t*uId,resource_id_t nId);
    ERRNO selectPostContentResource(resource_id_t uId,resource_id_t nId,datetime_t from,datetime_t to,sql::ResultSet**res);
    ERRNO selectMsgUsers(resource_id_t uId,sql::ResultSet**res);
    ERRNO isUserRefResource(resource_id_t uId,resource_id_t id);
    ERRNO insertNameResource(std::string name,resource_id_t*nameId,resource_mask_t resourceMask=RESOURCE_MASK_RESOURCE_NAME|RESOURCE_MASK_RESOURCE_AUTO_DELETE_REF0,DB_MEM_CACHE_TYPE type=DB_MEM_CACHE_TYPE_DFT);
    ERRNO insertPasswdResource(std::string passwd,resource_id_t*passwdId,resource_mask_t passwdMask=RESOURCE_MASK_PASSWD_DFT,resource_mask_t resourceMask=RESOURCE_MASK_RESOURCE_PASSWD|RESOURCE_MASK_RESOURCE_AUTO_DELETE_REF0,DB_MEM_CACHE_TYPE type=DB_MEM_CACHE_TYPE_DFT);
    ERRNO insertPostContentResource(resource_id_t*id,void*buf,size_t len,resource_mask_t mask=RESOURCE_MASK_POST_CONTENT_TYPE_DFT,resource_mask_t resourceMask=RESOURCE_MASK_RESOURCE_POST_CONTENT|RESOURCE_MASK_RESOURCE_AUTO_DELETE_REF0,DB_MEM_CACHE_TYPE type=DB_MEM_CACHE_TYPE_DFT);
    ERRNO insertUserResource(resource_id_t*userResourceId,resource_mask_t userResourceMask=RESOURCE_MASK_USER_DFT,resource_mask_t resourceMask=RESOURCE_MASK_RESOURCE_USER|RESOURCE_MASK_RESOURCE_AUTO_DELETE_REF0,DB_MEM_CACHE_TYPE type=DB_MEM_CACHE_TYPE_DFT);
    ERRNO userRefResource(resource_id_t userResourceId,resource_id_t resourceId,resource_mask_t resourceMask=RESOURCE_MASK_USER_DFT,DB_MEM_CACHE_TYPE type=DB_MEM_CACHE_TYPE_DFT);
    
    bool isErr(ERRNO err);
    bool isOk(ERRNO err);
    bool isWarn(ERRNO err);
  };
  extern Resource*g_resource;
}

#endif
