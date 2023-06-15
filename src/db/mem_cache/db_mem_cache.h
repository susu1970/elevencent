#ifndef _ELEVENCENT_DB_MEM_CACHE_H_
#define _ELEVENCENT_DB_MEM_CACHE_H_

#include<thread>
#include<unordered_map>
#include"resource_global.h"
#include"db_mapper.hpp"

namespace elevencent{
  class DbMemCache{
  public:    
    std::unordered_map<resource_id_t,DbMapper::Resource_Optimize*>m_resource;
    std::unordered_map<resource_id_t,std::unordered_map<resource_id_t,DbMapper::UserResource_Optimize*>>m_userResource;
    std::unordered_map<resource_id_t,DbMapper::NameResource_Optimize*>m_nameResource;
    std::unordered_map<resource_id_t,DbMapper::FileResource_Optimize*>m_fileResource;
    std::unordered_map<resource_id_t,DbMapper::PasswdResource_Optimize*>m_passwdResource;
    std::unordered_map<resource_id_t,DbMapper::PostResource_Optimize*>m_postResource;
    std::unordered_map<resource_id_t,DbMapper::PostContentResource_Optimize*>m_postContentResource;
    std::unordered_map<resource_type_t,size_t>m_cacheSizeLimit;
    std::unordered_map<resource_type_t,size_t>m_cacheSizeUsed;
    std::unordered_map<resource_type_t,DB_MEM_CACHE_REPLACEMENT>m_replacement;
  public:
    DbMemCache();
    DbMemCache(std::unordered_map<resource_type_t,size_t>cacheSizeLimit,std::unordered_map<resource_type_t,DB_MEM_CACHE_REPLACEMENT>replacement);
    const DbMapper::Resource_Optimize*getResource(resource_id_t resourceId);
    const DbMapper::UserResource_Optimize*getUserResource(resource_id_t userResourceId,resource_id_t resourceId);
    const DbMapper::UserResource_Optimize*getUserResourceByNameId(resource_id_t*userResourceId,resource_id_t nameId);    
    const DbMapper::NameResource_Optimize*getNameResource(resource_id_t resourceId);
    const DbMapper::NameResource_Optimize*getNameResource(std::string name,resource_id_t*resourceId=0);    
    const DbMapper::FileResource_Optimize*getFileResource(resource_id_t resourceId);
    const DbMapper::PasswdResource_Optimize*getPasswdResource(resource_id_t resourceId);
    const DbMapper::PostResource_Optimize*getPostResource(resource_id_t resourceId);
    const DbMapper::PostContentResource_Optimize*getPostContentResource(resource_id_t resourceId);
    const DbMapper::PasswdResource_Optimize*getPasswdResource(std::string passwd,resource_mask_t passwdMask,resource_id_t*resourceId);
    size_t getCacheSize();
    size_t getCacheSize(resource_type_t resourceType);
    size_t getCacheSize(resource_type_t resourceType,resource_id_t resourceId);
    size_t getCacheSize(resource_type_t resourceType,resource_id_t userResourceId,resource_id_t resourceId);
    bool updateResource(const DbMapper::Resource&resource);
    bool updateResource(const DbMapper::UserResource&resource);
    bool updateResource(const DbMapper::NameResource&resource);
    bool updateResource(const DbMapper::FileResource&resource);
    bool updateResource(const DbMapper::PasswdResource&resource);
    bool updateResource(const DbMapper::PostResource&resource);
    bool updateResource(const DbMapper::PostContentResource&resource);
    void deleteResource(resource_type_t resourceType,resource_id_t resourceId);
    void deleteResource(resource_type_t resourceType,resource_id_t userResourceId,resource_id_t resourceId);    
    void deleteResource(resource_type_t resourceType);
    void deleteResource();
    ~DbMemCache();
  };
}

#endif /* _MARIADB_MEM_CACHE_H_ */
