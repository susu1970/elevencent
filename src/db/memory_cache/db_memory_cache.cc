#include"db_memory_cache.h"

using namespace std;
using namespace elevencent;

DbMemoryCache::DbMemoryCache(){}
DbMemoryCache::DbMemoryCache(unordered_map<RESOURCE_TYPE,size_t>cacheSizeLimit){
  for(auto iter=cacheSizeLimit.begin();iter!=cacheSizeLimit.end();++iter)
    m_cacheSizeLimit[iter->first]=iter->second;
}
DbMemoryCache::DbMemoryCache(unordered_map<RESOURCE_TYPE,DB_MEMORY_CACHE_REPLACEMENT>replacement){
  for(auto iter=replacement.begin();iter!=replacement.end();++iter)
    m_replacement[iter->first]=iter->second;
}
DbMemoryCache::DbMemoryCache(unordered_map<RESOURCE_TYPE,size_t>cacheSizeLimit,unordered_map<RESOURCE_TYPE,DB_MEMORY_CACHE_REPLACEMENT>replacement){
  for(auto iter=cacheSizeLimit.begin();iter!=cacheSizeLimit.end();++iter)
    m_cacheSizeLimit[iter->first]=iter->second;
  for(auto iter=replacement.begin();iter!=replacement.end();++iter)
    m_replacement[iter->first]=iter->second;  
}
const DbMapper::Resource_Optimize*DbMemoryCache::getResource(resource_id_t resourceId){
  return m_resource.contains(resourceId)?m_resource[resourceId]:0;
}
const DbMapper::UserResource_Optimize*DbMemoryCache::getUserResource(resource_id_t userResourceId,resource_id_t resourceId){
  return m_userResource.contains(userResourceId)?(m_userResource[userResourceId].contains(resourceId)?m_userResource[userResourceId][resourceId]:0):0;
}
const DbMapper::NameResource_Optimize*DbMemoryCache::getNameResource(resource_id_t resourceId){
  return m_nameResource.contains(resourceId)?m_nameResource[resourceId]:0;
}
const DbMapper::FileResource_Optimize*DbMemoryCache::getFileResource(resource_id_t resourceId){
  return m_fileResource.contains(resourceId)?m_fileResource[resourceId]:0;
}
const DbMapper::PasswdResource_Optimize*DbMemoryCache::getPasswdResource(resource_id_t resourceId){
  return m_passwdResource.contains(resourceId)?m_passwdResource[resourceId]:0;
}
const DbMapper::PostResource_Optimize*DbMemoryCache::getPostResource(resource_id_t resourceId){
  return m_postResource.contains(resourceId)?m_postResource[resourceId]:0;
}
const DbMapper::PostContentResource_Optimize*DbMemoryCache::getPostContentResource(resource_id_t resourceId){
  return m_postContentResource.contains(resourceId)?m_postContentResource[resourceId]:0;
}
size_t DbMemoryCache::getCacheSize(){
  return getCacheSize(RESOURCE_TYPE::ALL);
}
size_t DbMemoryCache::getCacheSize(RESOURCE_TYPE resourceType){
  size_t ret=0;
  for(RESOURCE_TYPE type=RESOURCE_TYPE::START<<1;type!=RESOURCE_TYPE::END;type<<=1)
    if(resourceType&type)
      ret+=m_cacheSizeUsed[type];
  return ret;
}
size_t DbMemoryCache::getCacheSize(RESOURCE_TYPE resourceType,resource_id_t userResourceId,resource_id_t resourceId){
  if(!(resourceType&RESOURCE_TYPE::USER_RESOURCE)
     ||!m_userResource.contains(userResourceId)
     ||!m_userResource[userResourceId].contains(resourceId))
    return 0;
  return m_userResource[userResourceId][resourceId]->size();
}
bool DbMemoryCache::updateResource(const DbMapper::Resource&resource){
  size_t rsz=resource.DbMapper::Resource_Optimize::size();
  DbMapper::Resource_Optimize*resourceOpt;  
  size_t osz;  
  if(!m_resource.contains(resource.m_resourceId)){
    if(rsz>m_cacheSizeLimit[RESOURCE_TYPE::RESOURCE])
      return false;
    switch(m_replacement[RESOURCE_TYPE::RESOURCE]){
    case DB_MEMORY_CACHE_REPLACEMENT::RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE::RESOURCE]>m_cacheSizeLimit[RESOURCE_TYPE::RESOURCE]){
	auto iter=m_resource.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE::RESOURCE]-=resourceOpt->size();
	delete resourceOpt;
	m_resource.erase(iter);
      }
      break;
    default:
      return false;
    }
    m_resource[resource.m_resourceId]=new DbMapper::Resource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE::RESOURCE]+=rsz;
    return true;
  }
  resourceOpt=m_resource[resource.m_resourceId];
  osz=resourceOpt->size();
  if(m_cacheSizeUsed[RESOURCE_TYPE::RESOURCE]+(rsz-osz)>m_cacheSizeLimit[RESOURCE_TYPE::RESOURCE]){
    delete resourceOpt;
    m_resource.erase(resource.m_resourceId);
    m_cacheSizeUsed[RESOURCE_TYPE::RESOURCE]-=osz;
    if(rsz>m_cacheSizeLimit[RESOURCE_TYPE::RESOURCE])
      return false;
    switch(m_replacement[RESOURCE_TYPE::RESOURCE]){
    case DB_MEMORY_CACHE_REPLACEMENT::RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE::RESOURCE]>m_cacheSizeLimit[RESOURCE_TYPE::RESOURCE]){
	auto iter=m_resource.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE::RESOURCE]-=resourceOpt->size();
	delete resourceOpt;
	m_resource.erase(iter);
      }
    default:
      return false;      
    }
    m_resource[resource.m_resourceId]=new DbMapper::Resource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE::RESOURCE]+=rsz;
    return true;
  }
  m_cacheSizeUsed[RESOURCE_TYPE::RESOURCE]+=(rsz-osz);
  (*resourceOpt)=resource;
  return true;
}
bool DbMemoryCache::updateResource(const DbMapper::UserResource&resource){
  size_t rsz=resource.DbMapper::UserResource_Optimize::size();
  DbMapper::UserResource_Optimize*resourceOpt;  
  size_t osz;  
  if(!m_userResource.contains(resource.m_userResourceId)
     ||!m_userResource[resource.m_userResourceId].contains(resource.m_resourceId)){
    if(rsz>m_cacheSizeLimit[RESOURCE_TYPE::USER_RESOURCE])
      return false;
    switch(m_replacement[RESOURCE_TYPE::USER_RESOURCE]){
    case DB_MEMORY_CACHE_REPLACEMENT::RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE::USER_RESOURCE]>m_cacheSizeLimit[RESOURCE_TYPE::USER_RESOURCE]){
	auto userIter=m_userResource.begin();
	auto iter=userIter->second.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE::USER_RESOURCE]-=resourceOpt->size();
	delete resourceOpt;
	userIter->second.erase(iter);
	if(userIter->second.empty())
	  m_userResource.erase(userIter);
      }
      break;
    default:
      return false;
    }
    m_userResource[resource.m_userResourceId][resource.m_resourceId]=new DbMapper::UserResource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE::USER_RESOURCE]+=rsz;
    return true;
  }
  resourceOpt=m_userResource[resource.m_userResourceId][resource.m_resourceId];
  osz=resourceOpt->size();
  if(m_cacheSizeUsed[RESOURCE_TYPE::USER_RESOURCE]+(rsz-osz)>m_cacheSizeLimit[RESOURCE_TYPE::USER_RESOURCE]){
    delete resourceOpt;
    m_userResource[resource.m_userResourceId].erase(resource.m_resourceId);
    if(m_userResource[resource.m_userResourceId].empty())
      m_userResource.erase(resource.m_userResourceId);
    m_cacheSizeUsed[RESOURCE_TYPE::USER_RESOURCE]-=osz;
    if(rsz>m_cacheSizeLimit[RESOURCE_TYPE::USER_RESOURCE])
      return false;
    switch(m_replacement[RESOURCE_TYPE::USER_RESOURCE]){
    case DB_MEMORY_CACHE_REPLACEMENT::RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE::USER_RESOURCE]>m_cacheSizeLimit[RESOURCE_TYPE::USER_RESOURCE]){
	auto userIter=m_userResource.begin();
	auto iter=userIter->second.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE::USER_RESOURCE]-=resourceOpt->size();
	delete resourceOpt;
	userIter->second.erase(iter);
	if(userIter->second.empty())
	  m_userResource.erase(userIter);
      }
    default:
      return false;      
    }
    m_userResource[resource.m_userResourceId][resource.m_resourceId]=new DbMapper::UserResource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE::USER_RESOURCE]+=rsz;
    return true;
  }
  m_cacheSizeUsed[RESOURCE_TYPE::USER_RESOURCE]+=(rsz-osz);
  (*resourceOpt)=resource;
  return true;
}
bool DbMemoryCache::updateResource(const DbMapper::NameResource&resource){
  size_t rsz=resource.DbMapper::NameResource_Optimize::size();
  DbMapper::NameResource_Optimize*resourceOpt;  
  size_t osz;  
  if(!m_nameResource.contains(resource.m_resourceId)){
    if(rsz>m_cacheSizeLimit[RESOURCE_TYPE::NAME_RESOURCE])
      return false;
    switch(m_replacement[RESOURCE_TYPE::NAME_RESOURCE]){
    case DB_MEMORY_CACHE_REPLACEMENT::RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE::NAME_RESOURCE]>m_cacheSizeLimit[RESOURCE_TYPE::NAME_RESOURCE]){
	auto iter=m_nameResource.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE::NAME_RESOURCE]-=resourceOpt->size();
	delete resourceOpt;
	m_nameResource.erase(iter);
      }
      break;
    default:
      return false;
    }
    m_nameResource[resource.m_resourceId]=new DbMapper::NameResource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE::NAME_RESOURCE]+=rsz;
    return true;
  }
  resourceOpt=m_nameResource[resource.m_resourceId];
  osz=resourceOpt->size();
  if(m_cacheSizeUsed[RESOURCE_TYPE::NAME_RESOURCE]+(rsz-osz)>m_cacheSizeLimit[RESOURCE_TYPE::NAME_RESOURCE]){
    delete resourceOpt;
    m_nameResource.erase(resource.m_resourceId);
    m_cacheSizeUsed[RESOURCE_TYPE::NAME_RESOURCE]-=osz;
    if(rsz>m_cacheSizeLimit[RESOURCE_TYPE::NAME_RESOURCE])
      return false;
    switch(m_replacement[RESOURCE_TYPE::NAME_RESOURCE]){
    case DB_MEMORY_CACHE_REPLACEMENT::RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE::NAME_RESOURCE]>m_cacheSizeLimit[RESOURCE_TYPE::NAME_RESOURCE]){
	auto iter=m_nameResource.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE::NAME_RESOURCE]-=resourceOpt->size();
	delete resourceOpt;
	m_nameResource.erase(iter);
      }
    default:
      return false;      
    }
    m_nameResource[resource.m_resourceId]=new DbMapper::NameResource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE::NAME_RESOURCE]+=rsz;
    return true;
  }
  m_cacheSizeUsed[RESOURCE_TYPE::NAME_RESOURCE]+=(rsz-osz);
  (*resourceOpt)=resource;
  return true;
}
bool DbMemoryCache::updateResource(const DbMapper::PasswdResource&resource){
  size_t rsz=resource.DbMapper::PasswdResource_Optimize::size();
  DbMapper::PasswdResource_Optimize*resourceOpt;  
  size_t osz;  
  if(!m_passwdResource.contains(resource.m_resourceId)){
    if(rsz>m_cacheSizeLimit[RESOURCE_TYPE::PASSWD_RESOURCE])
      return false;
    switch(m_replacement[RESOURCE_TYPE::PASSWD_RESOURCE]){
    case DB_MEMORY_CACHE_REPLACEMENT::RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE::PASSWD_RESOURCE]>m_cacheSizeLimit[RESOURCE_TYPE::PASSWD_RESOURCE]){
	auto iter=m_passwdResource.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE::PASSWD_RESOURCE]-=resourceOpt->size();
	delete resourceOpt;
	m_passwdResource.erase(iter);
      }
      break;
    default:
      return false;
    }
    m_passwdResource[resource.m_resourceId]=new DbMapper::PasswdResource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE::PASSWD_RESOURCE]+=rsz;
    return true;
  }
  resourceOpt=m_passwdResource[resource.m_resourceId];
  osz=resourceOpt->size();
  if(m_cacheSizeUsed[RESOURCE_TYPE::PASSWD_RESOURCE]+(rsz-osz)>m_cacheSizeLimit[RESOURCE_TYPE::PASSWD_RESOURCE]){
    delete resourceOpt;
    m_passwdResource.erase(resource.m_resourceId);
    m_cacheSizeUsed[RESOURCE_TYPE::PASSWD_RESOURCE]-=osz;
    if(rsz>m_cacheSizeLimit[RESOURCE_TYPE::PASSWD_RESOURCE])
      return false;
    switch(m_replacement[RESOURCE_TYPE::PASSWD_RESOURCE]){
    case DB_MEMORY_CACHE_REPLACEMENT::RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE::PASSWD_RESOURCE]>m_cacheSizeLimit[RESOURCE_TYPE::PASSWD_RESOURCE]){
	auto iter=m_passwdResource.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE::PASSWD_RESOURCE]-=resourceOpt->size();
	delete resourceOpt;
	m_passwdResource.erase(iter);
      }
    default:
      return false;      
    }
    m_passwdResource[resource.m_resourceId]=new DbMapper::PasswdResource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE::PASSWD_RESOURCE]+=rsz;
    return true;
  }
  m_cacheSizeUsed[RESOURCE_TYPE::PASSWD_RESOURCE]+=(rsz-osz);
  (*resourceOpt)=resource;
  return true;
}
bool DbMemoryCache::updateResource(const DbMapper::PostResource&resource){
  size_t rsz=resource.DbMapper::PostResource_Optimize::size();
  DbMapper::PostResource_Optimize*resourceOpt;  
  size_t osz;  
  if(!m_postResource.contains(resource.m_postResourceId)){
    if(rsz>m_cacheSizeLimit[RESOURCE_TYPE::POST_RESOURCE])
      return false;
    switch(m_replacement[RESOURCE_TYPE::POST_RESOURCE]){
    case DB_MEMORY_CACHE_REPLACEMENT::RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE::POST_RESOURCE]>m_cacheSizeLimit[RESOURCE_TYPE::POST_RESOURCE]){
	auto iter=m_postResource.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE::POST_RESOURCE]-=resourceOpt->size();
	delete resourceOpt;
	m_postResource.erase(iter);
      }
      break;
    default:
      return false;
    }
    m_postResource[resource.m_postResourceId]=new DbMapper::PostResource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE::POST_RESOURCE]+=rsz;
    return true;
  }
  resourceOpt=m_postResource[resource.m_postResourceId];
  osz=resourceOpt->size();
  if(m_cacheSizeUsed[RESOURCE_TYPE::POST_RESOURCE]+(rsz-osz)>m_cacheSizeLimit[RESOURCE_TYPE::POST_RESOURCE]){
    delete resourceOpt;
    m_postResource.erase(resource.m_postResourceId);
    m_cacheSizeUsed[RESOURCE_TYPE::POST_RESOURCE]-=osz;
    if(rsz>m_cacheSizeLimit[RESOURCE_TYPE::POST_RESOURCE])
      return false;
    switch(m_replacement[RESOURCE_TYPE::POST_RESOURCE]){
    case DB_MEMORY_CACHE_REPLACEMENT::RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE::POST_RESOURCE]>m_cacheSizeLimit[RESOURCE_TYPE::POST_RESOURCE]){
	auto iter=m_postResource.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE::POST_RESOURCE]-=resourceOpt->size();
	delete resourceOpt;
	m_postResource.erase(iter);
      }
    default:
      return false;      
    }
    m_postResource[resource.m_postResourceId]=new DbMapper::PostResource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE::POST_RESOURCE]+=rsz;
    return true;
  }
  m_cacheSizeUsed[RESOURCE_TYPE::POST_RESOURCE]+=(rsz-osz);
  (*resourceOpt)=resource;
  return true;
}
bool DbMemoryCache::updateResource(const DbMapper::PostContentResource&resource){
  size_t rsz=resource.DbMapper::PostContentResource_Optimize::size();
  DbMapper::PostContentResource_Optimize*resourceOpt;  
  size_t osz;  
  if(!m_postContentResource.contains(resource.m_resourceId)){
    if(rsz>m_cacheSizeLimit[RESOURCE_TYPE::POST_CONTENT_RESOURCE])
      return false;
    switch(m_replacement[RESOURCE_TYPE::POST_CONTENT_RESOURCE]){
    case DB_MEMORY_CACHE_REPLACEMENT::RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE::POST_CONTENT_RESOURCE]>m_cacheSizeLimit[RESOURCE_TYPE::POST_CONTENT_RESOURCE]){
	auto iter=m_postContentResource.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE::POST_CONTENT_RESOURCE]-=resourceOpt->size();
	delete resourceOpt;
	m_postContentResource.erase(iter);
      }
      break;
    default:
      return false;
    }
    m_postContentResource[resource.m_resourceId]=new DbMapper::PostContentResource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE::POST_CONTENT_RESOURCE]+=rsz;
    return true;
  }
  resourceOpt=m_postContentResource[resource.m_resourceId];
  osz=resourceOpt->size();
  if(m_cacheSizeUsed[RESOURCE_TYPE::POST_CONTENT_RESOURCE]+(rsz-osz)>m_cacheSizeLimit[RESOURCE_TYPE::POST_CONTENT_RESOURCE]){
    delete resourceOpt;
    m_postContentResource.erase(resource.m_resourceId);
    m_cacheSizeUsed[RESOURCE_TYPE::POST_CONTENT_RESOURCE]-=osz;
    if(rsz>m_cacheSizeLimit[RESOURCE_TYPE::POST_CONTENT_RESOURCE])
      return false;
    switch(m_replacement[RESOURCE_TYPE::POST_CONTENT_RESOURCE]){
    case DB_MEMORY_CACHE_REPLACEMENT::RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE::POST_CONTENT_RESOURCE]>m_cacheSizeLimit[RESOURCE_TYPE::POST_CONTENT_RESOURCE]){
	auto iter=m_postContentResource.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE::POST_CONTENT_RESOURCE]-=resourceOpt->size();
	delete resourceOpt;
	m_postContentResource.erase(iter);
      }
    default:
      return false;      
    }
    m_postContentResource[resource.m_resourceId]=new DbMapper::PostContentResource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE::POST_CONTENT_RESOURCE]+=rsz;
    return true;
  }
  m_cacheSizeUsed[RESOURCE_TYPE::POST_CONTENT_RESOURCE]+=(rsz-osz);
  (*resourceOpt)=resource;
  return true;
}
void DbMemoryCache::deleteResource(RESOURCE_TYPE resourceType,resource_id_t resourceId){
  if((resourceType&RESOURCE_TYPE::USER_RESOURCE)
     &&m_userResource.contains(resourceId)){
    for(auto iter=m_userResource[resourceId].begin();iter!=m_userResource[resourceId].end();++iter){
      auto opt=iter->second;
      m_cacheSizeUsed[RESOURCE_TYPE::USER_RESOURCE]-=opt->size();
      delete opt;
    }
    m_userResource[resourceId].clear();
    m_userResource.erase(resourceId);
  }
  if((resourceType&RESOURCE_TYPE::RESOURCE)
     &&m_resource.contains(resourceId)){
    auto opt=m_resource[resourceId];
    m_cacheSizeUsed[RESOURCE_TYPE::RESOURCE]-=opt->size();
    delete opt;
    m_resource.erase(resourceId);
  }
  if((resourceType&RESOURCE_TYPE::NAME_RESOURCE)
     &&m_nameResource.contains(resourceId)){
    auto opt=m_nameResource[resourceId];
    m_cacheSizeUsed[RESOURCE_TYPE::NAME_RESOURCE]-=opt->size();
    delete opt;
    m_nameResource.erase(resourceId);
  }
  if((resourceType&RESOURCE_TYPE::FILE_RESOURCE)
     &&m_fileResource.contains(resourceId)){
    auto opt=m_fileResource[resourceId];
    m_cacheSizeUsed[RESOURCE_TYPE::FILE_RESOURCE]-=opt->size();
    delete opt;
    m_fileResource.erase(resourceId);
  }
  if((resourceType&RESOURCE_TYPE::PASSWD_RESOURCE)
     &&m_passwdResource.contains(resourceId)){
    auto opt=m_passwdResource[resourceId];
    m_cacheSizeUsed[RESOURCE_TYPE::PASSWD_RESOURCE]-=opt->size();
    delete opt;
    m_passwdResource.erase(resourceId);
  }
  if((resourceType&RESOURCE_TYPE::POST_RESOURCE)
     &&m_postResource.contains(resourceId)){
    auto opt=m_postResource[resourceId];
    m_cacheSizeUsed[RESOURCE_TYPE::POST_RESOURCE]-=opt->size();
    delete opt;
    m_postResource.erase(resourceId);
  }
  if((resourceType&RESOURCE_TYPE::POST_CONTENT_RESOURCE)
     &&m_postContentResource.contains(resourceId)){
    auto opt=m_postContentResource[resourceId];
    m_cacheSizeUsed[RESOURCE_TYPE::POST_CONTENT_RESOURCE]-=opt->size();
    delete opt;
    m_postContentResource.erase(resourceId);
  }
}
void DbMemoryCache::deleteResource(RESOURCE_TYPE resourceType,resource_id_t userResourceId,resource_id_t resourceId){
  if(!(resourceType&RESOURCE_TYPE::USER_RESOURCE)
     ||!m_userResource.contains(userResourceId)
     ||!m_userResource[userResourceId].contains(resourceId))
    return;
  auto userOpt=m_userResource[userResourceId][resourceId];
  m_cacheSizeUsed[RESOURCE_TYPE::USER_RESOURCE]-=userOpt->size();
  delete userOpt;
}
void DbMemoryCache::deleteResource(RESOURCE_TYPE resourceType){
  if(resourceType&RESOURCE_TYPE::USER_RESOURCE){
    for(auto userIter=m_userResource.begin();userIter!=m_userResource.end();++userIter)
      for(auto iter=userIter->second.begin();iter!=userIter->second.end();++iter)
	delete iter->second;
    m_userResource.clear();
    m_cacheSizeUsed[RESOURCE_TYPE::USER_RESOURCE]=0;
  }
  if(resourceType&RESOURCE_TYPE::RESOURCE){
    for(auto iter=m_resource.begin();iter!=m_resource.end();++iter)
      delete iter->second;
    m_resource.clear();
    m_cacheSizeUsed[RESOURCE_TYPE::RESOURCE]=0;
  }
  if(resourceType&RESOURCE_TYPE::NAME_RESOURCE){
    for(auto iter=m_nameResource.begin();iter!=m_nameResource.end();++iter)
      delete iter->second;
    m_nameResource.clear();
    m_cacheSizeUsed[RESOURCE_TYPE::NAME_RESOURCE]=0;
  }
  if(resourceType&RESOURCE_TYPE::FILE_RESOURCE){
    for(auto iter=m_fileResource.begin();iter!=m_fileResource.end();++iter)
      delete iter->second;
    m_fileResource.clear();
    m_cacheSizeUsed[RESOURCE_TYPE::FILE_RESOURCE]=0;
  }
  if(resourceType&RESOURCE_TYPE::PASSWD_RESOURCE){
    for(auto iter=m_passwdResource.begin();iter!=m_passwdResource.end();++iter)
      delete iter->second;
    m_passwdResource.clear();
    m_cacheSizeUsed[RESOURCE_TYPE::PASSWD_RESOURCE]=0;
  }
  if(resourceType&RESOURCE_TYPE::POST_RESOURCE){
    for(auto iter=m_postResource.begin();iter!=m_postResource.end();++iter)
      delete iter->second;
    m_postResource.clear();
    m_cacheSizeUsed[RESOURCE_TYPE::POST_RESOURCE]=0;
  }
  if(resourceType&RESOURCE_TYPE::POST_CONTENT_RESOURCE){
    for(auto iter=m_postContentResource.begin();iter!=m_postContentResource.end();++iter)
      delete iter->second;
    m_postContentResource.clear();
    m_cacheSizeUsed[RESOURCE_TYPE::POST_CONTENT_RESOURCE]=0;
  }
}
void DbMemoryCache::deleteResource(){
  return deleteResource(RESOURCE_TYPE::ALL);
}

DbMemoryCache::~DbMemoryCache(){
  deleteResource();
}
