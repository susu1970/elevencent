#ifndef ELEVENCENT_RESOURCE_H_
#define ELEVENCENT_RESOURCE_H_

#include<unordered_map>
#include"db_mapper.hpp"
#include"resource.4circle.h"
#include"db_memory_cache.4circle.h"

namespace elevencent{
  class DbMemoryCache;
  class Resource{
  protected:
    DbMemoryCache m_memoryCache;
    std::unordered_map<RESOURCE_TYPE,size_t>m_cacheSizeUsed;
    std::unordered_map<RESOURCE_TYPE,size_t>m_cacheSizeMax;
  public:
    virtual resource_id_t getResourceRef(resource_id_t resourceId,RESOURCE_CACHE_TYPE cacheType)=0;
    virtual bool createUserResource(resource_id_t userResourceId,resource_id_t resourceId,resource_id_t resourceMask,RESOURCE_CACHE_TYPE cacheType)=0;
    virtual bool createNameResource(resource_id_t resourceId,const std::string&name)=0;
    virtual bool createFileResource(resource_id_t resourceId,const std::string&fileName,std::string fileLocation);
    virtual bool createPasswdResource(resource_id_t resourceId,resource_mask_t resourceMask,std::string passwd,RESOURCE_CACHE_TYPE cacheType)=0;
    virtual bool createPostResource(resource_id_t resourceId,resource_mask_t resourceMask)=0;
    virtual bool createPostContentResource(resource_id_t resourceId,const std::string&postContent)=0;
    virtual void syncCache2Backend(RESOURCE_TYPE type)=0;
    virtual void syncCache2Backend()=0;    
    virtual ~Resource();
  };
}

#endif
