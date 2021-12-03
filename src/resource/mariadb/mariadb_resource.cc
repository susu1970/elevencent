#include"mariadb_resource.h"

using namespace std;
using namespace elevencent;
using namespace sql;

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
bool MariadbResource::consumeFreeResourceId(resource_id_t*resourceId){
  for(resource_id_t id=0;id<DB_MAX_RESOURCE_ID_NUM;++id)
    if(!isSetResourceIdBitMap(id)){
      if(resourceId)
	*resourceId=id;
      setResourceIdBitMap(id);
      return true;
    }
  return false;
}
bool MariadbResource::getFreeResourceId(resource_id_t*resourceId){
  for(resource_id_t id=0;id<DB_MAX_RESOURCE_ID_NUM;++id)
    if(!isSetResourceIdBitMap(id)){
      if(resourceId)
	*resourceId=id;
      return true;
    }
  return false;
}

bool MariadbResource::insertUserResource(resource_id_t*userResourceId,resource_mask_t userResourceMask,resource_mask_t resourceMask,DB_MEMORY_CACHE_TYPE type){
  resource_id_t freeUserResourceId;
  if(!getFreeResourceId(&freeUserResourceId))
    return false;
  return insertUserResource(userResourceId?*userResourceId=freeUserResourceId:freeUserResourceId,userResourceMask,resourceMask,type);
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
