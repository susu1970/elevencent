#include"mariadb_resource.h"

using namespace std;
using namespace elevencent;
using namespace sql;

MariadbResource::MariadbResource(ThreadPool&&thrPool,MariadbPool&&m_dbPool):m_thrPool(forward<ThreadPool>(thrPool)),m_dbPool(forward<MariadbPool>(m_dbPool)){
  
}

bool MariadbResource::createUserResource(resource_id_t userResourceId,resource_mask_t userMask,std::string passwd,resource_mask_t passwdMask,resource_mask_t userResourceMask,resource_mask_t passwdResourceMask){
  
}

bool MariadbResource::isExistResourceId(resource_id_t resourceId,DB_CACHE_TYPE type){
  switch(type){
  case DB_CACHE_TYPE::THROUGH:
    if(m_dbMemCache.getResource(resourceId))
      return true;
    {
      string sql="select (`resource_mask`,`resource_ref`) from `elevencent`.`resource` where `resource_id`="+toString(resourceId)+";";//resourceId(integer type) is safe to not cause sql injection in here;
      stmnt();
      ResultSet*res=stmnt.executeQuery();
      if(!res->next())
	return false;
      m_dbMemCache.updateResource(Resource(resourceId,res->getUInt64(1),res->getUInt(2)));
      return true;
    }
    break;
  default:
    return false;
  }
}
