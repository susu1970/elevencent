#include"mariadb_resource.h"

using namespace std;
using namespace elevencent;
using namespace sql;

void MariadbResource::insertPasswdResource(resource_id_t*passwdResourceId,std::string passwd,resource_mask_t passwdResourceMask,resource_mask_t resourceMask,DB_CACHE_TYPE type){
  switch(type){
  case DB_CACHE_TYPE::THROUGH:{
    if(passwdResourceMask&DB_PASSWD_RESOURCE_MASK::PLAIN){
      resource_id_t freePasswdResourceId=consumeFreeResourceId();
      if(passwdResourceId)*passwdResourceId=freePasswdResourceId;
      string passwdResourceIdStr=toString(freePasswdResourceId);
      string passwdResourceMaskStr=toString(passwdResourceMask);
      string resourceMaskStr=toString(resourceMask);
      PreparedStatement*stmnt(getConn()->prepareStatement(SQLString("insert into `elevencent`.`resource` (`resource_id`,`resource_mask`,`resource_ref`) values ("+passwdResourceIdStr+","+resourceMaskStr+",1);insert into `elevencent`.`passwd_resource` (`resource_id`,`resource_mask`,`passwd`) values ("+passwdResourceIdStr+","+passwdResourceMaskStr+",?)")));
      stmnt->setString(1,passwd);
      stmnt->executeUpdate();
      delete stmnt;
      m_dbMemCache.updateResource(Resource(freePasswdResourceId,resourceMask,1));
      m_dbMemCache.updateResource(PasswdResource(freePasswdResourceId,passwdResourceMask,passwd));
      setResourceIdBitMap(freePasswdResourceId);
      return true;
    }
  }
    break;
  default:
    break;
  }
  return false;
}
resource_id_t MariadbResource::consumeFreeResourceId(){
  for(resource_id_t id=0;id<DB_MAX_RESOURCE_ID_NUM;++id)
    if(!isSetResourceIdBitMap(id)){
      setResourceIdBitMap(id);
      return id;
    }
  return id;
}
bool MariadbResource::insertUserResource(resource_id_t*userResourceId,resource_mask_t userResourceMask,resource_mask_t resourceMask,DB_CACHE_TYPE type){
  return createUserResource(userResourceId?*userResourceId=consumeFreeResourceId():consumeFreeResourceId(),userResourceMask,resourceMask,type);
}
bool MariadbResource::insertUserResource(resource_id_t userResourceId,resource_mask_t userResourceMask,resource_mask_t resourceMask,DB_CACHE_TYPE type){
  if(isSetResourceIdBitMap(userResourceId))
    return false;
  switch(type){
  case DB_CACHE_TYPE::THROUGH:{
    string userResourceIdStr=toString(userResourceId);
    string userResourceMaskStr=toString(userResourceMask);
    string resourceMaskStr=toString(resourceMask);
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
  return m_dbPool.getConnection();
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
