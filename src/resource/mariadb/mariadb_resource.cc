#include"mariadb_resource.h"
#include<cstring>
#include"global.h"

using namespace std;
using namespace elevencent;
using namespace sql;

MariadbResource::MariadbResource(string initFile){
  INIReader ini(initFile);
  unordered_map<RESOURCE_TYPE,size_t>cacheSizeLimit;
  {
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
  }
  unordered_map<RESOURCE_TYPE,DB_MEMORY_CACHE_REPLACEMENT>replacement;
  {
    string rp;
    rp="dft";
    rp=ini.GetString(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_REPLACEMENT,MARIADB_RESOURCE_KEY_RESOURCE,rp);
    if(m_str2replacementUmap.contains(rp))
      replacement[RESOURCE_TYPE::RESOURCE]=m_str2replacementUmap[rp];
    rp="dft";
    rp=ini.GetString(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_REPLACEMENT,MARIADB_RESOURCE_KEY_USER_RESOURCE,rp);
    if(m_str2replacementUmap.contains(rp))
      replacement[RESOURCE_TYPE::USER_RESOURCE]=m_str2replacementUmap[rp];
    rp="dft";
    rp=ini.GetString(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_REPLACEMENT,MARIADB_RESOURCE_KEY_NAME_RESOURCE,rp);
    if(m_str2replacementUmap.contains(rp))
      replacement[RESOURCE_TYPE::NAME_RESOURCE]=m_str2replacementUmap[rp];
    rp="dft";
    rp=ini.GetString(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_REPLACEMENT,MARIADB_RESOURCE_KEY_FILE_RESOURCE,rp);
    if(m_str2replacementUmap.contains(rp))
      replacement[RESOURCE_TYPE::FILE_RESOURCE]=m_str2replacementUmap[rp];
    rp="dft";
    rp=ini.GetString(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_REPLACEMENT,MARIADB_RESOURCE_KEY_PASSWD_RESOURCE,rp);
    if(m_str2replacementUmap.contains(rp))
      replacement[RESOURCE_TYPE::PASSWD_RESOURCE]=m_str2replacementUmap[rp];
    rp="dft";
    rp=ini.GetString(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_REPLACEMENT,MARIADB_RESOURCE_KEY_POST_RESOURCE,rp);
    if(m_str2replacementUmap.contains(rp))
      replacement[RESOURCE_TYPE::POST_RESOURCE]=m_str2replacementUmap[rp];
    rp="dft";
    rp=ini.GetString(MARIADB_RESOURCE_MEMORY_CACHE_INI_SECTION_REPLACEMENT,MARIADB_RESOURCE_KEY_POST_CONTENT_RESOURCE,rp);
    if(m_str2replacementUmap.contains(rp))
      replacement[RESOURCE_TYPE::POST_CONTENT_RESOURCE]=m_str2replacementUmap[rp];
  }
  m_dbMemCache=new DbMemoryCache(cacheSizeLimit,replacement);
  unordered_map<string,string>properties;
  {
    string pr;
    pr="";
    pr=ini.GetString(MARIADB_RESOURCE_POOL_INI_SECTION_POOL,"hostName",pr);
    if(pr!="")
      properties["hostName"]=pr;
    pr="";
    pr=ini.GetString(MARIADB_RESOURCE_POOL_INI_SECTION_POOL,"userName",pr);
    if(pr!="")
      properties["userName"]=pr;
    pr="";
    pr=ini.GetString(MARIADB_RESOURCE_POOL_INI_SECTION_POOL,"password",pr);
    if(pr!="")
      properties["password"]=pr;
    pr="";
    pr=ini.GetString(MARIADB_RESOURCE_POOL_INI_SECTION_POOL,"schema",pr);
    if(pr!="")
      properties["schema"]=pr;
    pr="";
    pr=ini.GetString(MARIADB_RESOURCE_POOL_INI_SECTION_POOL,"port",pr);
    if(pr!="")
      properties["port"]=pr;
    pr="";
    pr=ini.GetString(MARIADB_RESOURCE_POOL_INI_SECTION_POOL,"OPT_RECONNECT",pr);
    if(pr!="")
      properties["OPT_RECONNECT"]=pr;
    pr="";
    pr=ini.GetString(MARIADB_RESOURCE_POOL_INI_SECTION_POOL,MARIADB_POOL_CACHE_NUM_KEY,pr);
    if(pr!="")
      properties[MARIADB_POOL_CACHE_NUM_KEY]=pr;
    pr="";
    pr=ini.GetString(MARIADB_RESOURCE_POOL_INI_SECTION_POOL,MARIADB_POOL_MAX_NUM_KEY,pr);
    if(pr!="")
      properties[MARIADB_POOL_MAX_NUM_KEY]=pr;
  }
  m_dbPool=new MariadbPool(properties);
  m_conn=MariadbPool::getConn(m_dbPool);
  //max_allowed_packet
  {
    PreparedStatement*stmnt(m_conn->prepareStatement(SQLString("show variables like 'max_allowed_packet'")));
    ResultSet*res=stmnt->executeQuery();
    res->next();
    m_maxAllowedPacket=res->getUInt64(2);
    delete stmnt;
  }
  //init resource_id bitmap
  memset(m_resourceIdBitMap,0,sizeof(m_resourceIdBitMap));
  const unsigned int k_limit=m_maxAllowedPacket/sizeof(resource_id_t);
  resource_id_t count=0;
  {
    PreparedStatement*stmnt(m_conn->prepareStatement(SQLString("select count(*) from `elevencent`.`resource`")));
    ResultSet*res=stmnt->executeQuery();
    if(res->next())
      count=res->getUInt64(1);
    delete stmnt;
  }
  if(count<=k_limit){
    PreparedStatement*stmnt(m_conn->prepareStatement(SQLString("select `resource_id` from `elevencent`.`resource`")));
    ResultSet*res=stmnt->executeQuery();
    while(res->next())
      setResourceIdBitMap(res->getUInt64(1));
    delete stmnt;
  }else{
    resource_id_t minResourceId=0;
    while(count){
      PreparedStatement*stmnt(m_conn->prepareStatement(SQLString("select `resource_id` from `elevencent`.`resource` where `resource_id` >= "+to_string(minResourceId)+" limit "+to_string(k_limit))));
      ResultSet*res=stmnt->executeQuery();
      while(res->next()){
	--count;
	setResourceIdBitMap(minResourceId=res->getUInt64(1));
      }
      delete stmnt;
      ++minResourceId;
    }
  }
  for(m_resourceIdFreeStart=0;isSetResourceIdBitMap(m_resourceIdFreeStart)&&m_resourceIdFreeStart<DB_MAX_RESOURCE_ID_NUM;++m_resourceIdFreeStart);
}

bool MariadbResource::insertUserResource(resource_id_t userResourceId,resource_mask_t userResourceMask,resource_mask_t resourceMask,DB_MEMORY_CACHE_TYPE type){
  if(isSetResourceIdBitMap(userResourceId)||userResourceId>DB_MAX_RESOURCE_ID_NUM)
    return false;
  switch(type){
  case DB_MEMORY_CACHE_TYPE::THROUGH:{
    auto conn=getConn(false);
    {
      PreparedStatement*stmnt(conn->prepareStatement(SQLString("insert into `elevencent`.`resource` (`resource_id`,`resource_mask`,`resource_ref`) values (?,?,1)")));
      stmnt->setUInt64(1,userResourceId);
      stmnt->setUInt64(2,resourceMask);
      if(stmnt->executeUpdate()!=1){
	conn->rollback();
	delete stmnt;
	return false;
      }
      delete stmnt;
    }
    {
      PreparedStatement*stmnt(conn->prepareStatement(SQLString("insert into `elevencent`.`user_resource` (`user_resource_id`,`resource_id`,`resource_mask`) values (?,?,?);")));
      stmnt->setUInt64(1,userResourceId);
      stmnt->setUInt64(2,userResourceId);
      stmnt->setUInt64(3,userResourceMask);
      if(stmnt->executeUpdate()!=1){
	conn->rollback();
	delete stmnt;
	return false;
      }
      delete stmnt;
    }    
    conn->commit();
    consumeResourceId(userResourceId);
    DbMapper::UserResource userResource(userResourceId,userResourceId,userResourceMask,0,0);
    DbMapper::Resource resource(userResourceId,resourceMask,1);
    m_dbMemCache->updateResource(userResource);
    m_dbMemCache->updateResource(resource);
    return true;
  }
    break;
  default:
    return false;
  }
  return false;
}
bool MariadbResource::insertUserResource(resource_id_t*userResourceId,resource_mask_t userResourceMask,resource_mask_t resourceMask,DB_MEMORY_CACHE_TYPE type){
  resource_id_t freeResourceId;
  if(!peekFreeResourceId(&freeResourceId))
    return false;
  if(userResourceId)
    *userResourceId=freeResourceId;
  return insertUserResource(freeResourceId,userResourceMask,resourceMask,type);
}
bool MariadbResource::insertPasswdResource(std::string passwd,resource_id_t passwdResourceId,resource_mask_t passwdResourceMask,resource_mask_t resourceMask,DB_MEMORY_CACHE_TYPE type){
  if(isSetResourceIdBitMap(passwdResourceId)||passwdResourceId>DB_MAX_RESOURCE_ID_NUM)
    return false;
  switch(type){
  case DB_MEMORY_CACHE_TYPE::THROUGH:{
    if(passwdResourceMask&DB_PASSWD_RESOURCE_MASK::PLAIN){
      if(passwd.size()>MAX_RESOURCE_PASSWD)
	return false;
      if(m_dbMemCache->getPasswdResource(passwd,passwdResourceMask,0))
	return false;
      auto conn=getConn(false);
      {
	PreparedStatement*stmnt(conn->prepareStatement(SQLString("select `resource_id` from `elevencent`.`passwd_resource` where `passwd`=? and `resource_mask`=?")));
	stmnt->setString(1,passwd);
	stmnt->setUInt64(2,passwdResourceMask);
	ResultSet*res=stmnt->executeQuery();
	conn->commit();
	if(res->next()){
	  delete res;
	  delete stmnt;
	  return false;
	}
	delete res;	
	delete stmnt;	
      }
      {
	PreparedStatement*stmnt(conn->prepareStatement(SQLString("insert into `elevencent`.`resource` (`resource_id`,`resource_mask`,`resource_ref`) values (?,?,0)")));
	stmnt->setUInt64(1,passwdResourceId);
	stmnt->setUInt64(2,resourceMask);
	if(stmnt->executeUpdate()!=1){
	  conn->rollback();
	  delete stmnt;
	  return false;
	}
	delete stmnt;
      }      
      {
	PreparedStatement*stmnt(conn->prepareStatement(SQLString("insert into `elevencent`.`passwd_resource` (`resource_id`,`passwd`,`resource_mask`) values (?,?,?);")));
	stmnt->setUInt64(1,passwdResourceId);
	stmnt->setString(2,passwd);
	stmnt->setUInt64(3,passwdResourceMask);
	if(stmnt->executeUpdate()!=1){
	  conn->rollback();
	  delete stmnt;
	  return false;
	}
	delete stmnt;
      }    
      conn->commit();
      consumeResourceId(passwdResourceId);
      m_dbMemCache->updateResource(DbMapper::PasswdResource(passwdResourceId,passwdResourceMask,passwd));
      m_dbMemCache->updateResource(DbMapper::Resource(passwdResourceId,resourceMask,0));
      return true;
    }
  }
    break;
  default:
    return false;
  }
  return false;
}
bool MariadbResource::userRefResource(resource_id_t userResourceId,resource_id_t resourceId,resource_mask_t resourceMask,DB_MEMORY_CACHE_TYPE type){
  if(userResourceId>DB_MAX_RESOURCE_ID_NUM||resourceId>DB_MAX_RESOURCE_ID_NUM||!isSetResourceIdBitMap(userResourceId)||!isSetResourceIdBitMap(resourceId))
    return false;
  switch(type){
  case DB_MEMORY_CACHE_TYPE::THROUGH:{
    const DbMapper::Resource_Optimize*resourceOpt=m_dbMemCache->getResource(userResourceId);
    if(resourceOpt&&!(resourceOpt->m_resourceMask&(resource_mask_t)(DB_RESOURCE_MASK::USER_RESOURCE)))
      return false;
    if(m_dbMemCache->getUserResource(userResourceId,resourceId))
      return false;
    auto conn=getConn(false);
    {
      PreparedStatement*stmnt(conn->prepareStatement(SQLString("select `resource_mask`,`resource_ref` from `elevencent`.`resource` where `resource_id`=?")));
      stmnt->setUInt64(1,userResourceId);
      ResultSet*res=stmnt->executeQuery();
      conn->commit();
      res->next();
      uint64_t mask=res->getUInt64(1);
      uint64_t ref=res->getUInt64(2);
      if(!(mask&(resource_mask_t)(DB_RESOURCE_MASK::USER_RESOURCE))){
	delete res;
	delete stmnt;
	return false;
      }
      m_dbMemCache->updateResource(DbMapper::Resource(userResourceId,mask,ref));
      delete res;
      delete stmnt;
    }
    {
      PreparedStatement*stmnt(conn->prepareStatement(SQLString("select `user_resource_id` from `elevencent`.`user_resource` where `user_resource_id`=? and `resource_id`=?")));
      stmnt->setUInt64(1,userResourceId);
      stmnt->setUInt64(2,resourceId);
      ResultSet*res=stmnt->executeQuery();
      conn->commit();
      if(res->next()){
	delete res;
	delete stmnt;
	return false;
      }
      delete res;
      delete stmnt;
    }
    {
      PreparedStatement*stmnt(conn->prepareStatement(SQLString("update `elevencent`.`resource` set `resource_ref`=`resource_ref`+1 where `resource_id`=?")));
      stmnt->setUInt64(1,resourceId);
      if(stmnt->executeUpdate()!=1){
	conn->rollback();
	delete stmnt;
	return false;
      }
      delete stmnt;
    }
    {
      PreparedStatement*stmnt(conn->prepareStatement(SQLString("insert into `elevencent`.`user_resource`(`user_resource_id`,`resource_id`,`resource_mask`) values(?,?,?)")));
      stmnt->setUInt64(1,userResourceId);
      stmnt->setUInt64(2,resourceId);
      stmnt->setUInt64(3,resourceMask);      
      if(stmnt->executeUpdate()!=1){
	conn->rollback();
	delete stmnt;
	return false;
      }
      delete stmnt;      
    }
    conn->commit();
    resourceOpt=m_dbMemCache->getResource(resourceId);
    if(resourceOpt)
      m_dbMemCache->updateResource(DbMapper::Resource(resourceId,resourceOpt->m_resourceMask,resourceOpt->m_resourceRef+1));
    m_dbMemCache->updateResource(DbMapper::UserResource(userResourceId,resourceId,resourceMask,0,0));
    return true;
  }
    break;
  default:
    break;
  }
  return false;
}
bool MariadbResource::insertPasswdResource(std::string passwd,resource_id_t*passwdResourceId,resource_mask_t passwdResourceMask,resource_mask_t resourceMask,DB_MEMORY_CACHE_TYPE type){
  resource_id_t freeResourceId;
  if(!peekFreeResourceId(&freeResourceId))
    return false;
  if(passwdResourceId)
    *passwdResourceId=freeResourceId;
  return insertPasswdResource(passwd,freeResourceId,passwdResourceMask,resourceMask,type);
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
bool MariadbResource::consumeResourceId(resource_id_t resourceId){
  setResourceIdBitMap(resourceId);
  if(resourceId!=m_resourceIdFreeStart)
    return true;
  resource_id_t id=m_resourceIdFreeStart;
  DB_RESOURCE_ID_LOOP_INCRE(m_resourceIdFreeStart);    
  while(m_resourceIdFreeStart!=id){
    if(!isSetResourceIdBitMap(m_resourceIdFreeStart))
      return true;
    DB_RESOURCE_ID_LOOP_INCRE(m_resourceIdFreeStart);          
  }
  return false;
}
Connection*MariadbResource::getConn(){
  return m_conn;
}
Connection*MariadbResource::getConn(bool autoCommit){
  m_conn->setAutoCommit(autoCommit);  
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
MariadbResource::~MariadbResource(){
  MariadbPool::recycleConn(m_dbPool,m_conn);
  delete m_dbPool;
  delete m_dbMemCache;
}
