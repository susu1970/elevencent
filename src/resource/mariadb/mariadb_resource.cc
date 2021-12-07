#include"mariadb_resource.h"

using namespace std;
using namespace elevencent;
using namespace sql;


void MariadbResource::insertPasswdResource(resource_id_t*passwdResourceId,std::string passwd,resource_mask_t passwdResourceMask,resource_mask_t resourceMask,DB_CACHE_TYPE type){
  switch(type){
  case DB_CACHE_TYPE::THROUGH:{
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
      m_dbMemCache.updateResource(Resource(freePasswdResourceId,resourceMask,1));
      m_dbMemCache.updateResource(PasswdResource(freePasswdResourceId,passwdResourceMask,passwd));
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
  for(;id!=resourceIdFreeStart;DB_RESOURCE_ID_LOOP_INCRE(id)){
    if(!isSetResourceIdBitMap(id)){
      if(resourceId)
	*resourceId=id;
      return true;
    }
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
  for(resource_id_t id=m_resourceIdFreeStart;id!=resourceIdFreeStart;DB_RESOURCE_ID_LOOP_INCRE(id)){
    if(!isSetResourceIdBitMap(id)){
      setResourceIdBitMap(id);
      if(resourceId)
	*resourceId=id;
      m_resourceIdFreeStart=id;
      DB_RESOURCE_ID_LOOP_INCRE(m_resourceIdFreeStart);
      return true;
    }
  }
  return false;
}
bool MariadbResource::insertUserResource(resource_id_t*userResourceId,resource_mask_t userResourceMask,resource_mask_t resourceMask,DB_CACHE_TYPE type){
  resource_id_t id;
  if(!peekFreeResourceId(&id))
    return false;
  if(userResourceId)
    *userResourceId=id;
  return createUserResource(id,userResourceMask,resourceMask,type);
}
bool MariadbResource::insertUserResource(resource_id_t userResourceId,resource_mask_t userResourceMask,resource_mask_t resourceMask,DB_CACHE_TYPE type){
  if(isSetResourceIdBitMap(userResourceId))
    return false;
  switch(type){
  case DB_CACHE_TYPE::THROUGH:{
    string userResourceIdStr=to_string(userResourceId);
    string userResourceMaskStr=to_string(userResourceMask);
    string resourceMaskStr=to_string(resourceMask);
    PreparedStatement*stmnt(getConn()->prepareStatement(SQLString("insert into `elevencent`.`resource` (`resource_id`,`resource_mask`,`resource_ref`) values ("+userResourceIdStr+","+resourceMaskStr+",1);insert into `elevencent`.`user_resource` (`user_resource_id`,`resource_id`,`resource_mask`) values ("+userResourceIdStr+","+userResourceIdStr+","+userResourceMaskStr+")")));
    stmnt->executeUpdate();
    delete stmnt;
    setResourceIdBitMap(userResourceId);
    m_dbMemCache.updateResource(UserResource(userResourceId,userResourceId,userResourceMask,0,0));//optimize happend here,if you really want `create_time` or `update_time`,you should fetch them from the database-backend!
    m_dbMemCache.updateResource(Resource(userResourceId,resourceMask,1));
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
void setResourceIdBitMap(resource_id_t resourceId){
  m_resourceIdBitMap[resourceId/8]|=(1<<(resourceId%8));
}
void unSetResourceIdBitMap(resource_id_t resourceId){
  m_resourceIdBitMap[resourceId/8]&=(~(1<<(resourceId%8)));
}
bool isSetResourceIdBitMap(resource_id_t resourceId){
  m_resourceIdBitMap[resourceId/8]&(1<<(resourceId%8));
}
