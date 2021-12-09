#include"mariadb_resource.h"
#include<unordered_map>

using namespace std;
using namespace elevencent;
using namespace sql;

MariadbResource::MariadbResource(std::string initFile){
  INIReader ini(initFile);
  unordered_map<RESOURCE_TYPE,size_t>cacheSizeLimit;
  long sz;
  sz=-1;
  sz=ini.GetInteger(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_LIMIT_SIZE,MARIADB_RESOURCE_KEY_RESOURCE,sz);
  if(sz>=0)
    cacheSizeLimit[RESOURCE_TYPE::RESOURCE]=sz;
  sz=-1;
  sz=ini.GetInteger(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_LIMIT_SIZE,MARIADB_RESOURCE_KEY_USER_RESOURCE,sz);
  if(sz>=0)
    cacheSizeLimit[RESOURCE_TYPE::USER_RESOURCE]=sz;
  sz=-1;
  sz=ini.GetInteger(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_LIMIT_SIZE,MARIADB_RESOURCE_KEY_NAME_RESOURCE,sz);
  if(sz>=0)
    cacheSizeLimit[RESOURCE_TYPE::NAME_RESOURCE]=sz;
  sz=-1;
  sz=ini.GetInteger(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_LIMIT_SIZE,MARIADB_RESOURCE_KEY_FILE_RESOURCE,sz);
  if(sz>=0)
    cacheSizeLimit[RESOURCE_TYPE::FILE_RESOURCE]=sz;
  sz=-1;
  sz=ini.GetInteger(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_LIMIT_SIZE,MARIADB_RESOURCE_KEY_PASSWD_RESOURCE,sz);
  if(sz>=0)
    cacheSizeLimit[RESOURCE_TYPE::PASSWD_RESOURCE]=sz;
  sz=-1;
  sz=ini.GetInteger(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_LIMIT_SIZE,MARIADB_RESOURCE_KEY_POST_RESOURCE,sz);
  if(sz>=0)
    cacheSizeLimit[RESOURCE_TYPE::POST_RESOURCE]=sz;        
  sz=-1;
  sz=ini.GetInteger(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_LIMIT_SIZE,MARIADB_RESOURCE_KEY_POST_CONTENT_RESOURCE,sz);
  if(sz>=0)
    cacheSizeLimit[RESOURCE_TYPE::POST_CONTENT_RESOURCE]=sz;
  unordered_map<RESOURCE_TYPE,DB_MEMORY_CACHE_REPLACEMENT>replacement;
  unordered_map<string,DB_MEMORY_CACHE_REPLACEMENT>replacementUmap{
    {"rr",DB_MEMORY_CACHE_REPLACEMENT::RR},
    {"lru",DB_MEMORY_CACHE_REPLACEMENT::LRU},
    {"fifo",DB_MEMORY_CACHE_REPLACEMENT::FIFO},
    {"dft",DB_MEMORY_CACHE_REPLACEMENT::DFT}
  };
  string rp;
  rp="";
  rp=ini.GetInteger(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_REPLACEMENT,MARIADB_RESOURCE_KEY_RESOURCE,rp);
  if(rp!="")
    replacement[RESOURCE_TYPE::RESOURCE]=replacementUmap[rp];
  rp="";
  rp=ini.GetInteger(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_REPLACEMENT,MARIADB_RESOURCE_KEY_USER_RESOURCE,rp);
  if(rp!="")
    replacement[RESOURCE_TYPE::USER_RESOURCE]=replacementUmap[rp];
  rp="";
  rp=ini.GetInteger(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_REPLACEMENT,MARIADB_RESOURCE_KEY_NAME_RESOURCE,rp);
  if(rp!="")
    replacement[RESOURCE_TYPE::NAME_RESOURCE]=replacementUmap[rp];  
  rp="";
  rp=ini.GetInteger(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_REPLACEMENT,MARIADB_RESOURCE_KEY_FILE_RESOURCE,rp);
  if(rp!="")
    replacement[RESOURCE_TYPE::FILE_RESOURCE]=replacementUmap[rp];  
  rp="";
  rp=ini.GetInteger(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_REPLACEMENT,MARIADB_RESOURCE_KEY_PASSWD_RESOURCE,rp);
  if(rp!="")
    replacement[RESOURCE_TYPE::PASSWD_RESOURCE]=replacementUmap[rp];  
  rp="";
  rp=ini.GetInteger(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_REPLACEMENT,MARIADB_RESOURCE_KEY_POST_RESOURCE,rp);
  if(rp!="")
    replacement[RESOURCE_TYPE::POST_RESOURCE]=replacementUmap[rp];  
  rp="";
  rp=ini.GetInteger(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_REPLACEMENT,MARIADB_RESOURCE_KEY_POST_CONTENT_RESOURCE,rp);
  if(rp!="")
    replacement[RESOURCE_TYPE::POST_CONTENT_RESOURCE]=replacementUmap[rp];  
}
bool MariadbResource::insertPasswdResource(resource_id_t*passwdResourceId,std::string passwd,resource_mask_t passwdResourceMask,resource_mask_t resourceMask,DB_MEMORY_CACHE_TYPE type){
  switch(type){
  case DB_MEMORY_CACHE_TYPE::THROUGH:{
    if(passwdResourceMask&DB_PASSWD_RESOURCE_MASK::PLAIN){
      resource_id_t freePasswdResourceId;
      if(!consumeFreeResourceId(&freePasswdResourceId))
	return false;
      if(passwdResourceId)*passwdResourceId=freePasswdResourceId;
      string passwdResourceIdStr=to_string(freePasswdResourceId);
      string passwdResourceMaskStr=to_string(passwdResourceMask);
      string resourceMaskStr=to_string(resourceMask);
      PreparedStatement*stmnt(getConn()->prepareStatement(SQLString("insert into `elevencent`.`resource` (`resource_id`,`resource_mask`,`resource_ref`) values ("+passwdResourceIdStr+","+resourceMaskStr+",1);insert into `elevencent`.`passwd_resource` (`resource_id`,`resource_mask`,`passwd`) values ("+passwdResourceIdStr+","+passwdResourceMaskStr+",?)")));
      stmnt->setString(1,passwd);
      stmnt->executeUpdate();
      delete stmnt;
      m_dbMemCache.updateResource(DbMapper::Resource(freePasswdResourceId,resourceMask,1));
      m_dbMemCache.updateResource(DbMapper::PasswdResource(freePasswdResourceId,passwdResourceMask,passwd));
      return true;
    }
  }
    break;
  default:
    break;
  }
  return false;
}
bool MariadbResource::peekFreeResourceId(resource_id_t*resourceId){
  if(!isSetResourceIdBitMap(m_resourceIdFreeStart)){
    if(resourceId)
      *resourceId=m_resourceIdFreeStart;
    return true;
  }
  resource_id_t resourceIdFreeStart=m_resourceIdFreeStart,id=m_resourceIdFreeStart;
  DB_RESOURCE_ID_LOOP_INCRE(id);
  for(;id!=resourceIdFreeStart;){
    if(!isSetResourceIdBitMap(id)){
      if(resourceId)
	*resourceId=id;
      return true;
    }
    DB_RESOURCE_ID_LOOP_INCRE(id);    
  }
  return false;
}
bool MariadbResource::consumeFreeResourceId(resource_id_t*resourceId){
  if(!isSetResourceIdBitMap(m_resourceIdFreeStart)){
    setResourceIdBitMap(m_resourceIdFreeStart);
    if(resourceId)
      *resourceId=m_resourceIdFreeStart;
    DB_RESOURCE_ID_LOOP_INCRE(m_resourceIdFreeStart);    
    return true;
  }
  resource_id_t resourceIdFreeStart=m_resourceIdFreeStart;
  DB_RESOURCE_ID_LOOP_INCRE(m_resourceIdFreeStart);
  for(resource_id_t id=m_resourceIdFreeStart;id!=resourceIdFreeStart;){
    if(!isSetResourceIdBitMap(id)){
      setResourceIdBitMap(id);
      if(resourceId)
	*resourceId=id;
      m_resourceIdFreeStart=id;
      DB_RESOURCE_ID_LOOP_INCRE(m_resourceIdFreeStart);
      return true;
    }
    DB_RESOURCE_ID_LOOP_INCRE(id);    
  }
  return false;
}
bool MariadbResource::insertUserResource(resource_id_t*userResourceId,resource_mask_t userResourceMask,resource_mask_t resourceMask,DB_MEMORY_CACHE_TYPE type){
  resource_id_t id;
  if(!peekFreeResourceId(&id))
    return false;
  if(userResourceId)
    *userResourceId=id;
  return insertUserResource(id,userResourceMask,resourceMask,type);
}

bool MariadbResource::insertUserResource(resource_id_t userResourceId,resource_mask_t userResourceMask,resource_mask_t resourceMask,DB_MEMORY_CACHE_TYPE type){
  if(isSetResourceIdBitMap(userResourceId))
    return false;
  switch(type){
  case DB_MEMORY_CACHE_TYPE::THROUGH:{
    string userResourceIdStr=to_string(userResourceId);
    string userResourceMaskStr=to_string(userResourceMask);
    string resourceMaskStr=to_string(resourceMask);
    PreparedStatement*stmnt(getConn()->prepareStatement(SQLString("insert into `elevencent`.`resource` (`resource_id`,`resource_mask`,`resource_ref`) values ("+userResourceIdStr+","+resourceMaskStr+",1);insert into `elevencent`.`user_resource` (`user_resource_id`,`resource_id`,`resource_mask`) values ("+userResourceIdStr+","+userResourceIdStr+","+userResourceMaskStr+")")));
    stmnt->executeUpdate();
    delete stmnt;
    setResourceIdBitMap(userResourceId);
    m_dbMemCache.updateResource(DbMapper::UserResource(userResourceId,userResourceId,userResourceMask,0,0));//optimize happend here,if you really want `create_time` or `update_time`,you should fetch them from the database-backend!
    m_dbMemCache.updateResource(DbMapper::Resource(userResourceId,resourceMask,1));
    return true;
  }
    break;
  default:
    break;
  }
  return false;
}
Connection*MariadbResource::getConn(){
  return m_conn;
}
void MariadbResource::setResourceIdBitMap(resource_id_t resourceId){
  m_resourceIdBitMap[resourceId/8]|=(((resource_id_t)1)<<(resourceId%8));
}
void MariadbResource::unSetResourceIdBitMap(resource_id_t resourceId){
  m_resourceIdBitMap[resourceId/8]&=(~(((resource_id_t)1)<<(resourceId%8)));
}
bool MariadbResource::isSetResourceIdBitMap(resource_id_t resourceId){
  return m_resourceIdBitMap[resourceId/8]&(((resource_id_t)1)<<(resourceId%8));
}
