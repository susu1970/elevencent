#ifndef _ELEVENCENT_MEMORY_CACHE_H_
#define _ELEVENCENT_MEMORY_CACHE_H_

#include<thread>
#include<unordered_map>
#include"db_memory_cache.4circle.h"
#include"db_mapper.hpp"
#include"resource.4circle.h"

namespace elevencent{
  class DbMemoryCache{
  public:    
    std::unordered_map<resource_id_t,DbMapper::Resource_Optimize*>m_resource;
    std::unordered_map<resource_id_t,std::unordered_map<resource_id_t,DbMapper::UserResource_Optimize*>>m_userResource;
    std::unordered_map<resource_id_t,DbMapper::NameResource_Optimize*>m_nameResource;
    std::unordered_map<resource_id_t,DbMapper::FileResource_Optimize*>m_fileResource;
    std::unordered_map<resource_id_t,DbMapper::PasswdResource_Optimize*>m_passwdResource;
    std::unordered_map<resource_id_t,DbMapper::PostResource_Optimize*>m_postResource;
    std::unordered_map<resource_id_t,DbMapper::PostContentResource_Optimize*>m_postContentResource;
    std::unordered_map<RESOURCE_TYPE,size_t>m_cacheSizeLimit{
      {RESOURCE_TYPE::RESOURCE,1073741824},
      {RESOURCE_TYPE::USER_RESOURCE,1073741824},
      {RESOURCE_TYPE::NAME_RESOURCE,536870912},
      {RESOURCE_TYPE::FILE_RESOURCE,268435456},
      {RESOURCE_TYPE::PASSWD_RESOURCE,0},
      {RESOURCE_TYPE::POST_RESOURCE,0},
      {RESOURCE_TYPE::POST_CONTENT_RESOURCE,0}      
    };
    std::unordered_map<RESOURCE_TYPE,size_t>m_cacheSizeUsed;
    std::unordered_map<RESOURCE_TYPE,DB_MEMORY_CACHE_REPLACEMENT>m_replacement{
      {RESOURCE_TYPE::RESOURCE,DB_MEMORY_CACHE_REPLACEMENT::RR},
      {RESOURCE_TYPE::USER_RESOURCE,DB_MEMORY_CACHE_REPLACEMENT::RR},
      {RESOURCE_TYPE::NAME_RESOURCE,DB_MEMORY_CACHE_REPLACEMENT::RR},
      {RESOURCE_TYPE::FILE_RESOURCE,DB_MEMORY_CACHE_REPLACEMENT::RR},
      {RESOURCE_TYPE::PASSWD_RESOURCE,DB_MEMORY_CACHE_REPLACEMENT::RR},
      {RESOURCE_TYPE::POST_RESOURCE,DB_MEMORY_CACHE_REPLACEMENT::RR},
      {RESOURCE_TYPE::POST_CONTENT_RESOURCE,DB_MEMORY_CACHE_REPLACEMENT::RR}            
    };
  public:
    DbMemoryCache();
    DbMemoryCache(std::unordered_map<RESOURCE_TYPE,size_t>cacheSizeLimit);
    DbMemoryCache(std::unordered_map<RESOURCE_TYPE,DB_MEMORY_CACHE_REPLACEMENT>replacement);    
    DbMemoryCache(std::unordered_map<RESOURCE_TYPE,size_t>cacheSizeLimit,std::unordered_map<RESOURCE_TYPE,DB_MEMORY_CACHE_REPLACEMENT>replacement);    
    const DbMapper::Resource_Optimize*getResource(resource_id_t resourceId);
    const DbMapper::UserResource_Optimize*getUserResource(resource_id_t userResourceId,resource_id_t resourceId);
    const DbMapper::NameResource_Optimize*getNameResource(resource_id_t resourceId);
    const DbMapper::FileResource_Optimize*getFileResource(resource_id_t resourceId);
    const DbMapper::PasswdResource_Optimize*getPasswdResource(resource_id_t resourceId);
    const DbMapper::PasswdResource_Optimize*getPasswdResource(std::string passwd,resource_mask_t passwdMask);
    const DbMapper::PostResource_Optimize*getPostResource(resource_id_t resourceId);
    const DbMapper::PostContentResource_Optimize*getPostContentResource(resource_id_t resourceId);
    bool getPasswdResourceId(std::string passwd,resource_mask_t passwdMask,resource_id_t*resourceId);

    size_t getCacheSize();
    size_t getCacheSize(RESOURCE_TYPE resourceType);
    size_t getCacheSize(RESOURCE_TYPE resourceType,resource_id_t resourceId);
    size_t getCacheSize(RESOURCE_TYPE resourceType,resource_id_t userResourceId,resource_id_t resourceId);

    bool updateResource(const DbMapper::Resource&resource);
    bool updateResource(const DbMapper::UserResource&resource);
    bool updateResource(const DbMapper::NameResource&resource);
    bool updateResource(const DbMapper::FileResource&resource);
    bool updateResource(const DbMapper::PasswdResource&resource);
    bool updateResource(const DbMapper::PostResource&resource);
    bool updateResource(const DbMapper::PostContentResource&resource);

    void deleteResource(RESOURCE_TYPE resourceType,resource_id_t resourceId);
    void deleteResource(RESOURCE_TYPE resourceType,resource_id_t userResourceId,resource_id_t resourceId);    
    void deleteResource(RESOURCE_TYPE resourceType);
    void deleteResource();

    ~DbMemoryCache();
  };
}

#endif /* _MARIADB_MEMORY_CACHE_H_ */
