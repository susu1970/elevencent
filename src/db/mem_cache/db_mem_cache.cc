#include"db_mem_cache.h"

using namespace std;
using namespace elevencent;

DbMemCache::DbMemCache(){}
DbMemCache::DbMemCache(unordered_map<resource_type_t,size_t>cacheSizeLimit,unordered_map<resource_type_t,DB_MEM_CACHE_REPLACEMENT>replacement){
  for(auto iter=cacheSizeLimit.begin();iter!=cacheSizeLimit.end();++iter)
    m_cacheSizeLimit[iter->first]=iter->second;
  for(auto iter=replacement.begin();iter!=replacement.end();++iter)
    m_replacement[iter->first]=iter->second;  
}
const DbMapper::Resource_Optimize*DbMemCache::getResource(resource_id_t resourceId){
  return m_resource.contains(resourceId)?m_resource[resourceId]:0;
}
const DbMapper::UserResource_Optimize*DbMemCache::getUserResource(resource_id_t userResourceId,resource_id_t resourceId){
  return m_userResource.contains(userResourceId)?(m_userResource[userResourceId].contains(resourceId)?m_userResource[userResourceId][resourceId]:0):0;
}
const DbMapper::UserResource_Optimize*DbMemCache::getUserResourceByNameId(resource_id_t*userResourceId,resource_id_t nameId){
  for(auto&iter:m_userResource){
    if(iter.second.contains(nameId)){
      if(userResourceId)
	*userResourceId=iter.first;
      return iter.second[nameId];
    }
  }
  return 0;
}
const DbMapper::NameResource_Optimize*DbMemCache::getNameResource(string name,resource_id_t*resourceId){
  for(auto&iter:m_nameResource){
    auto obj=iter.second;
    if(obj->m_name==name){
      if(resourceId)
	*resourceId=iter.first;
      return obj;
    }
  }
  return 0;
}
const DbMapper::NameResource_Optimize*DbMemCache::getNameResource(resource_id_t resourceId){
  return m_nameResource.contains(resourceId)?m_nameResource[resourceId]:0;
}
const DbMapper::FileResource_Optimize*DbMemCache::getFileResource(resource_id_t resourceId){
  return m_fileResource.contains(resourceId)?m_fileResource[resourceId]:0;
}
const DbMapper::PasswdResource_Optimize*DbMemCache::getPasswdResource(resource_id_t resourceId){
  return m_passwdResource.contains(resourceId)?m_passwdResource[resourceId]:0;
}
const DbMapper::PasswdResource_Optimize*DbMemCache::getPasswdResource(std::string passwd,resource_mask_t passwdMask,resource_id_t*resourceId){
  if(passwdMask&RESOURCE_MASK_PASSWD_PLAIN){
    for(auto&iter:m_passwdResource){
      DbMapper::PasswdResource_Optimize*obj=iter.second;
      if(obj->m_resourceMask==passwdMask&&obj->m_passwd==passwd){
	if(resourceId)
	  *resourceId=iter.first;
	return obj;
      }
    }
  }
  return 0;
}
const DbMapper::PostResource_Optimize*DbMemCache::getPostResource(resource_id_t resourceId){
  return m_postResource.contains(resourceId)?m_postResource[resourceId]:0;
}
const DbMapper::PostContentResource_Optimize*DbMemCache::getPostContentResource(resource_id_t resourceId){
  return m_postContentResource.contains(resourceId)?m_postContentResource[resourceId]:0;
}
size_t DbMemCache::getCacheSize(){
  return getCacheSize(RESOURCE_TYPE_ALL);
}
size_t DbMemCache::getCacheSize(resource_type_t resourceType){
  size_t ret=0;
  for(resource_type_t type=RESOURCE_TYPE_START<<1;type!=RESOURCE_TYPE_END;type<<=1)
    if(resourceType&type)
      ret+=m_cacheSizeUsed[type];
  return ret;
}
size_t DbMemCache::getCacheSize(resource_type_t resourceType,resource_id_t userResourceId,resource_id_t resourceId){
  if(!(resourceType&RESOURCE_TYPE_USER)
     ||!m_userResource.contains(userResourceId)
     ||!m_userResource[userResourceId].contains(resourceId))
    return 0;
  return m_userResource[userResourceId][resourceId]->size();
}
bool DbMemCache::updateResource(const DbMapper::Resource&resource){
  size_t rsz=resource.DbMapper::Resource_Optimize::size();
  if(rsz>m_cacheSizeLimit[RESOURCE_TYPE_RESOURCE])
    return false;  
  DbMapper::Resource_Optimize*resourceOpt;  
  size_t osz;  
  if(!m_resource.contains(resource.m_resourceId)){
    switch(m_replacement[RESOURCE_TYPE_RESOURCE]){
    case DB_MEM_CACHE_REPLACEMENT_RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE_RESOURCE]>m_cacheSizeLimit[RESOURCE_TYPE_RESOURCE]){
	auto iter=m_resource.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE_RESOURCE]-=resourceOpt->size();
	delete resourceOpt;
	m_resource.erase(iter);
      }
      break;
    default:
      return false;
    }
    m_resource[resource.m_resourceId]=new DbMapper::Resource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE_RESOURCE]+=rsz;
    return true;
  }
  resourceOpt=m_resource[resource.m_resourceId];
  osz=resourceOpt->size();
  if(m_cacheSizeUsed[RESOURCE_TYPE_RESOURCE]+(rsz-osz)>m_cacheSizeLimit[RESOURCE_TYPE_RESOURCE]){
    delete resourceOpt;
    m_resource.erase(resource.m_resourceId);
    m_cacheSizeUsed[RESOURCE_TYPE_RESOURCE]-=osz;
    switch(m_replacement[RESOURCE_TYPE_RESOURCE]){
    case DB_MEM_CACHE_REPLACEMENT_RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE_RESOURCE]>m_cacheSizeLimit[RESOURCE_TYPE_RESOURCE]){
	auto iter=m_resource.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE_RESOURCE]-=resourceOpt->size();
	delete resourceOpt;
	m_resource.erase(iter);
      }
      break;
    default:
      return false;      
    }
    m_resource[resource.m_resourceId]=new DbMapper::Resource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE_RESOURCE]+=rsz;
    return true;
  }
  m_cacheSizeUsed[RESOURCE_TYPE_RESOURCE]+=(rsz-osz);
  (*resourceOpt)=resource;
  return true;
}
bool DbMemCache::updateResource(const DbMapper::UserResource&resource){
  size_t rsz=resource.DbMapper::UserResource_Optimize::size();
  if(rsz>m_cacheSizeLimit[RESOURCE_TYPE_USER])
    return false;  
  DbMapper::UserResource_Optimize*resourceOpt;  
  size_t osz;  
  if(!m_userResource.contains(resource.m_userResourceId)
     ||!m_userResource[resource.m_userResourceId].contains(resource.m_resourceId)){
    switch(m_replacement[RESOURCE_TYPE_USER]){
    case DB_MEM_CACHE_REPLACEMENT_RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE_USER]>m_cacheSizeLimit[RESOURCE_TYPE_USER]){
	auto userIter=m_userResource.begin();
	auto iter=userIter->second.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE_USER]-=resourceOpt->size();
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
    m_cacheSizeUsed[RESOURCE_TYPE_USER]+=rsz;
    return true;
  }
  resourceOpt=m_userResource[resource.m_userResourceId][resource.m_resourceId];
  osz=resourceOpt->size();
  if(m_cacheSizeUsed[RESOURCE_TYPE_USER]+(rsz-osz)>m_cacheSizeLimit[RESOURCE_TYPE_USER]){
    delete resourceOpt;
    m_userResource[resource.m_userResourceId].erase(resource.m_resourceId);
    if(m_userResource[resource.m_userResourceId].empty())
      m_userResource.erase(resource.m_userResourceId);
    m_cacheSizeUsed[RESOURCE_TYPE_USER]-=osz;
    switch(m_replacement[RESOURCE_TYPE_USER]){
    case DB_MEM_CACHE_REPLACEMENT_RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE_USER]>m_cacheSizeLimit[RESOURCE_TYPE_USER]){
	auto userIter=m_userResource.begin();
	auto iter=userIter->second.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE_USER]-=resourceOpt->size();
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
    m_cacheSizeUsed[RESOURCE_TYPE_USER]+=rsz;
    return true;
  }
  m_cacheSizeUsed[RESOURCE_TYPE_USER]+=(rsz-osz);
  (*resourceOpt)=resource;
  return true;
}
bool DbMemCache::updateResource(const DbMapper::NameResource&resource){
  size_t rsz=resource.DbMapper::NameResource_Optimize::size();
  if(rsz>m_cacheSizeLimit[RESOURCE_TYPE_NAME])
    return false;
  DbMapper::NameResource_Optimize*resourceOpt;  
  size_t osz;  
  if(!m_nameResource.contains(resource.m_resourceId)){
    switch(m_replacement[RESOURCE_TYPE_NAME]){
    case DB_MEM_CACHE_REPLACEMENT_RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE_NAME]>m_cacheSizeLimit[RESOURCE_TYPE_NAME]){
	auto iter=m_nameResource.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE_NAME]-=resourceOpt->size();
	delete resourceOpt;
	m_nameResource.erase(iter);
      }
      break;
    default:
      return false;
    }
    m_nameResource[resource.m_resourceId]=new DbMapper::NameResource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE_NAME]+=rsz;
    return true;
  }
  resourceOpt=m_nameResource[resource.m_resourceId];
  osz=resourceOpt->size();
  if(m_cacheSizeUsed[RESOURCE_TYPE_NAME]+(rsz-osz)>m_cacheSizeLimit[RESOURCE_TYPE_NAME]){
    delete resourceOpt;
    m_nameResource.erase(resource.m_resourceId);
    m_cacheSizeUsed[RESOURCE_TYPE_NAME]-=osz;
    switch(m_replacement[RESOURCE_TYPE_NAME]){
    case DB_MEM_CACHE_REPLACEMENT_RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE_NAME]>m_cacheSizeLimit[RESOURCE_TYPE_NAME]){
	auto iter=m_nameResource.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE_NAME]-=resourceOpt->size();
	delete resourceOpt;
	m_nameResource.erase(iter);
      }
      break;
    default:
      return false;      
    }
    m_nameResource[resource.m_resourceId]=new DbMapper::NameResource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE_NAME]+=rsz;
    return true;
  }
  m_cacheSizeUsed[RESOURCE_TYPE_NAME]+=(rsz-osz);
  (*resourceOpt)=resource;
  return true;
}
bool DbMemCache::updateResource(const DbMapper::PasswdResource&resource){
  size_t rsz=resource.DbMapper::PasswdResource_Optimize::size();
  if(rsz>m_cacheSizeLimit[RESOURCE_TYPE_PASSWD])
    return false;
  DbMapper::PasswdResource_Optimize*resourceOpt;  
  size_t osz;  
  if(!m_passwdResource.contains(resource.m_resourceId)){
    switch(m_replacement[RESOURCE_TYPE_PASSWD]){
    case DB_MEM_CACHE_REPLACEMENT_RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE_PASSWD]>m_cacheSizeLimit[RESOURCE_TYPE_PASSWD]){
	auto iter=m_passwdResource.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE_PASSWD]-=resourceOpt->size();
	delete resourceOpt;
	m_passwdResource.erase(iter);
      }
      break;
    default:
      return false;
    }
    m_passwdResource[resource.m_resourceId]=new DbMapper::PasswdResource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE_PASSWD]+=rsz;
    return true;
  }
  resourceOpt=m_passwdResource[resource.m_resourceId];
  osz=resourceOpt->size();
  if(m_cacheSizeUsed[RESOURCE_TYPE_PASSWD]+(rsz-osz)>m_cacheSizeLimit[RESOURCE_TYPE_PASSWD]){
    delete resourceOpt;
    m_passwdResource.erase(resource.m_resourceId);
    m_cacheSizeUsed[RESOURCE_TYPE_PASSWD]-=osz;
    switch(m_replacement[RESOURCE_TYPE_PASSWD]){
    case DB_MEM_CACHE_REPLACEMENT_RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE_PASSWD]>m_cacheSizeLimit[RESOURCE_TYPE_PASSWD]){
	auto iter=m_passwdResource.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE_PASSWD]-=resourceOpt->size();
	delete resourceOpt;
	m_passwdResource.erase(iter);
      }
      break;
    default:
      return false;      
    }
    m_passwdResource[resource.m_resourceId]=new DbMapper::PasswdResource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE_PASSWD]+=rsz;
    return true;
  }
  m_cacheSizeUsed[RESOURCE_TYPE_PASSWD]+=(rsz-osz);
  (*resourceOpt)=resource;
  return true;
}
bool DbMemCache::updateResource(const DbMapper::PostResource&resource){
  size_t rsz=resource.DbMapper::PostResource_Optimize::size();
  if(rsz>m_cacheSizeLimit[RESOURCE_TYPE_POST])
    return false;
  DbMapper::PostResource_Optimize*resourceOpt;  
  size_t osz;  
  if(!m_postResource.contains(resource.m_postResourceId)){
    switch(m_replacement[RESOURCE_TYPE_POST]){
    case DB_MEM_CACHE_REPLACEMENT_RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE_POST]>m_cacheSizeLimit[RESOURCE_TYPE_POST]){
	auto iter=m_postResource.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE_POST]-=resourceOpt->size();
	delete resourceOpt;
	m_postResource.erase(iter);
      }
      break;
    default:
      return false;
    }
    m_postResource[resource.m_postResourceId]=new DbMapper::PostResource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE_POST]+=rsz;
    return true;
  }
  resourceOpt=m_postResource[resource.m_postResourceId];
  osz=resourceOpt->size();
  if(m_cacheSizeUsed[RESOURCE_TYPE_POST]+(rsz-osz)>m_cacheSizeLimit[RESOURCE_TYPE_POST]){
    delete resourceOpt;
    m_postResource.erase(resource.m_postResourceId);
    m_cacheSizeUsed[RESOURCE_TYPE_POST]-=osz;
    switch(m_replacement[RESOURCE_TYPE_POST]){
    case DB_MEM_CACHE_REPLACEMENT_RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE_POST]>m_cacheSizeLimit[RESOURCE_TYPE_POST]){
	auto iter=m_postResource.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE_POST]-=resourceOpt->size();
	delete resourceOpt;
	m_postResource.erase(iter);
      }
      break;
    default:
      return false;      
    }
    m_postResource[resource.m_postResourceId]=new DbMapper::PostResource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE_POST]+=rsz;
    return true;
  }
  m_cacheSizeUsed[RESOURCE_TYPE_POST]+=(rsz-osz);
  (*resourceOpt)=resource;
  return true;
}
bool DbMemCache::updateResource(const DbMapper::PostContentResource&resource){
  size_t rsz=resource.DbMapper::PostContentResource_Optimize::size();
  if(rsz>m_cacheSizeLimit[RESOURCE_TYPE_POST_CONTENT])
    return false;
  DbMapper::PostContentResource_Optimize*resourceOpt;  
  size_t osz;  
  if(!m_postContentResource.contains(resource.m_resourceId)){
    switch(m_replacement[RESOURCE_TYPE_POST_CONTENT]){
    case DB_MEM_CACHE_REPLACEMENT_RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE_POST_CONTENT]>m_cacheSizeLimit[RESOURCE_TYPE_POST_CONTENT]){
	auto iter=m_postContentResource.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE_POST_CONTENT]-=resourceOpt->size();
	delete resourceOpt;
	m_postContentResource.erase(iter);
      }
      break;
    default:
      return false;
    }
    m_postContentResource[resource.m_resourceId]=new DbMapper::PostContentResource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE_POST_CONTENT]+=rsz;
    return true;
  }
  resourceOpt=m_postContentResource[resource.m_resourceId];
  osz=resourceOpt->size();
  if(m_cacheSizeUsed[RESOURCE_TYPE_POST_CONTENT]+(rsz-osz)>m_cacheSizeLimit[RESOURCE_TYPE_POST_CONTENT]){
    delete resourceOpt;
    m_postContentResource.erase(resource.m_resourceId);
    m_cacheSizeUsed[RESOURCE_TYPE_POST_CONTENT]-=osz;
    switch(m_replacement[RESOURCE_TYPE_POST_CONTENT]){
    case DB_MEM_CACHE_REPLACEMENT_RR:
      while(rsz+m_cacheSizeUsed[RESOURCE_TYPE_POST_CONTENT]>m_cacheSizeLimit[RESOURCE_TYPE_POST_CONTENT]){
	auto iter=m_postContentResource.begin();
	resourceOpt=iter->second;
	m_cacheSizeUsed[RESOURCE_TYPE_POST_CONTENT]-=resourceOpt->size();
	delete resourceOpt;
	m_postContentResource.erase(iter);
      }
      break;
    default:
      return false;      
    }
    m_postContentResource[resource.m_resourceId]=new DbMapper::PostContentResource_Optimize(resource);
    m_cacheSizeUsed[RESOURCE_TYPE_POST_CONTENT]+=rsz;
    return true;
  }
  m_cacheSizeUsed[RESOURCE_TYPE_POST_CONTENT]+=(rsz-osz);
  (*resourceOpt)=resource;
  return true;
}
void DbMemCache::deleteResource(resource_type_t resourceType,resource_id_t resourceId){
  if((resourceType&RESOURCE_TYPE_USER)
     &&m_userResource.contains(resourceId)){
    for(auto iter=m_userResource[resourceId].begin();iter!=m_userResource[resourceId].end();++iter){
      auto opt=iter->second;
      m_cacheSizeUsed[RESOURCE_TYPE_USER]-=opt->size();
      delete opt;
    }
    m_userResource[resourceId].clear();
    m_userResource.erase(resourceId);
  }
  if((resourceType&RESOURCE_TYPE_RESOURCE)
     &&m_resource.contains(resourceId)){
    auto opt=m_resource[resourceId];
    m_cacheSizeUsed[RESOURCE_TYPE_RESOURCE]-=opt->size();
    delete opt;
    m_resource.erase(resourceId);
  }
  if((resourceType&RESOURCE_TYPE_NAME)
     &&m_nameResource.contains(resourceId)){
    auto opt=m_nameResource[resourceId];
    m_cacheSizeUsed[RESOURCE_TYPE_NAME]-=opt->size();
    delete opt;
    m_nameResource.erase(resourceId);
  }
  if((resourceType&RESOURCE_TYPE_FILE)
     &&m_fileResource.contains(resourceId)){
    auto opt=m_fileResource[resourceId];
    m_cacheSizeUsed[RESOURCE_TYPE_FILE]-=opt->size();
    delete opt;
    m_fileResource.erase(resourceId);
  }
  if((resourceType&RESOURCE_TYPE_PASSWD)
     &&m_passwdResource.contains(resourceId)){
    auto opt=m_passwdResource[resourceId];
    m_cacheSizeUsed[RESOURCE_TYPE_PASSWD]-=opt->size();
    delete opt;
    m_passwdResource.erase(resourceId);
  }
  if((resourceType&RESOURCE_TYPE_POST)
     &&m_postResource.contains(resourceId)){
    auto opt=m_postResource[resourceId];
    m_cacheSizeUsed[RESOURCE_TYPE_POST]-=opt->size();
    delete opt;
    m_postResource.erase(resourceId);
  }
  if((resourceType&RESOURCE_TYPE_POST_CONTENT)
     &&m_postContentResource.contains(resourceId)){
    auto opt=m_postContentResource[resourceId];
    m_cacheSizeUsed[RESOURCE_TYPE_POST_CONTENT]-=opt->size();
    delete opt;
    m_postContentResource.erase(resourceId);
  }
}
void DbMemCache::deleteResource(resource_type_t resourceType,resource_id_t userResourceId,resource_id_t resourceId){
  if(!(resourceType&RESOURCE_TYPE_USER)
     ||!m_userResource.contains(userResourceId)
     ||!m_userResource[userResourceId].contains(resourceId))
    return;
  auto userOpt=m_userResource[userResourceId][resourceId];
  m_cacheSizeUsed[RESOURCE_TYPE_USER]-=userOpt->size();
  delete userOpt;
  m_userResource[userResourceId].erase(resourceId);
}
void DbMemCache::deleteResource(resource_type_t resourceType){
  if(resourceType&RESOURCE_TYPE_USER){
    for(auto userIter=m_userResource.begin();userIter!=m_userResource.end();++userIter)
      for(auto iter=userIter->second.begin();iter!=userIter->second.end();++iter)
	delete iter->second;
    m_userResource.clear();
    m_cacheSizeUsed[RESOURCE_TYPE_USER]=0;
  }
  if(resourceType&RESOURCE_TYPE_RESOURCE){
    for(auto iter=m_resource.begin();iter!=m_resource.end();++iter)
      delete iter->second;
    m_resource.clear();
    m_cacheSizeUsed[RESOURCE_TYPE_RESOURCE]=0;
  }
  if(resourceType&RESOURCE_TYPE_NAME){
    for(auto iter=m_nameResource.begin();iter!=m_nameResource.end();++iter)
      delete iter->second;
    m_nameResource.clear();
    m_cacheSizeUsed[RESOURCE_TYPE_NAME]=0;
  }
  if(resourceType&RESOURCE_TYPE_FILE){
    for(auto iter=m_fileResource.begin();iter!=m_fileResource.end();++iter)
      delete iter->second;
    m_fileResource.clear();
    m_cacheSizeUsed[RESOURCE_TYPE_FILE]=0;
  }
  if(resourceType&RESOURCE_TYPE_PASSWD){
    for(auto iter=m_passwdResource.begin();iter!=m_passwdResource.end();++iter)
      delete iter->second;
    m_passwdResource.clear();
    m_cacheSizeUsed[RESOURCE_TYPE_PASSWD]=0;
  }
  if(resourceType&RESOURCE_TYPE_POST){
    for(auto iter=m_postResource.begin();iter!=m_postResource.end();++iter)
      delete iter->second;
    m_postResource.clear();
    m_cacheSizeUsed[RESOURCE_TYPE_POST]=0;
  }
  if(resourceType&RESOURCE_TYPE_POST_CONTENT){
    for(auto iter=m_postContentResource.begin();iter!=m_postContentResource.end();++iter)
      delete iter->second;
    m_postContentResource.clear();
    m_cacheSizeUsed[RESOURCE_TYPE_POST_CONTENT]=0;
  }
}
void DbMemCache::deleteResource(){
  return deleteResource(RESOURCE_TYPE_ALL);
}

DbMemCache::~DbMemCache(){
  deleteResource();
}
