#include"mariadb_resource.h"

using namespace std;
using namespace elevencent;
using namespace sql;

void MariadbResource::insertPasswdResource(resource_id_t resourceId,resource_mask_t resourceMask,std::string passwd){
  PreparedStatement*stmnt(getConn()->prepareStatement(SQLString("insert into `elevencent`.`passwd_resource` (`resource_id`,`resource_mask`,`passwd`) values ("+toString(resourceId)+","+toString(resourceMask)+",?) ")));
  stmnt->setString(1,passwd);
  stmnt->executeUpdate();
  delete stmnt;
}

bool MariadbResource::createUserResource(resource_id_t userResourceId,resource_mask_t userMask,std::string passwd,resource_mask_t passwdMask,resource_mask_t userResourceMask,resource_mask_t passwdResourceMask,DB_CACHE_TYPE type){
  if(m_resourceIdBitMap[userResourceId])
    return false;
  switch(type){
  case DB_CACHE_TYPE::THROUGH:
    if(passwdMask==DB_PASSWD_RESOURCE_MASK::PLAIN){
      resource_id_t passwdResourceId;
      const PasswdResource_Optimize*passwdResource_Opt;
      if(!m_dbMemCache.getPasswdResourceId(passwd,passwdMask,&passwdResourceId)){
	PreparedStatement*stmnt(getConn()->prepareStatement(SQLString("select `resource_id` from `elevencent`.`passwd_resource` where passwd=? and `resource_mask`="+toString(passwdMask)+")")));
	stmnt->setString(1,passwd);
	ResultSet*res=stmnt->executeQuery();
	if(res->next()){
	  m_dbMemCache.updateResource(PasswdResource(passwdResourceId=res->getUInt(1),passwdMask,passwd));
	  delete stmnt;
	  increResourceRef(passwdResourceId);
	}else{
	  delete stmnt;
	  insertPasswdResource(passwdResourceId=consumeFreeResourceId(),passwdMask,passwd);
	  insertResource(passwdResourceId,passwdResourceMask,1);
	}
	m_dbMemCache.updateResource(PasswdResource(passwdResourceId,passwdMask,passwd));
      }
      insertUserResource(userResourceId,passwdResourceId,userMask);
      insertResource(userResourceId,userResourceMask,1);
    }
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

void setResourceIdBitMap(resource_id_t resourceId){
  m_resourceIdBitMap[resourceId/8]|=(1<<(resourceId%8));
}
void unSetResourceIdBitMap(resource_id_t resourceId){
  m_resourceIdBitMap[resourceId/8]&=(~(1<<(resourceId%8)));
}
bool isSetResourceIdBitMap(resource_id_t resourceId){
  m_resourceIdBitMap[resourceId/8]&(1<<(resourceId%8));
}
