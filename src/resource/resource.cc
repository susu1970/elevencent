#include"resource.h"

using namespace std;
using namespace elevencent;
using namespace sql;
using namespace elevencent::ResourceLimit;
using ScopeConn=MariadbPool::ScopeConn;
static unordered_map<string,DB_MEM_CACHE_REPLACEMENT>s_str2replacementUmap{
  {"rr",DB_MEM_CACHE_REPLACEMENT_RR},
  {"lru",DB_MEM_CACHE_REPLACEMENT_LRU},
  {"fifo",DB_MEM_CACHE_REPLACEMENT_FIFO},
  {"dft",DB_MEM_CACHE_REPLACEMENT_DFT}
};
Resource::Resource(string initFile){
  INIReader ini(initFile);
  unordered_map<resource_type_t,size_t>cacheSizeLimit;
  {
    long sz;
    sz=-1;
    sz=ini.GetInteger("mem_cache_limit_size","resource",sz);
    if(sz>=0)
      cacheSizeLimit[RESOURCE_TYPE_RESOURCE]=sz;
    sz=-1;
    sz=ini.GetInteger("mem_cache_limit_size","user",sz);
    if(sz>=0)
      cacheSizeLimit[RESOURCE_TYPE_USER]=sz;
    sz=-1;
    sz=ini.GetInteger("mem_cache_limit_size","name",sz);
    if(sz>=0)
      cacheSizeLimit[RESOURCE_TYPE_NAME]=sz;
    sz=-1;
    sz=ini.GetInteger("mem_cache_limit_size","file",sz);
    if(sz>=0)
      cacheSizeLimit[RESOURCE_TYPE_FILE]=sz;
    sz=-1;
    sz=ini.GetInteger("mem_cache_limit_size","passwd",sz);
    if(sz>=0)
      cacheSizeLimit[RESOURCE_TYPE_PASSWD]=sz;
    sz=-1;
    sz=ini.GetInteger("mem_cache_limit_size","post",sz);
    if(sz>=0)
      cacheSizeLimit[RESOURCE_TYPE_POST]=sz;        
    sz=-1;
    sz=ini.GetInteger("mem_cache_limit_size","post_content",sz);
    if(sz>=0)
      cacheSizeLimit[RESOURCE_TYPE_POST_CONTENT]=sz;
  }
  unordered_map<resource_type_t,DB_MEM_CACHE_REPLACEMENT>replacement;
  {
    string rp;
    rp="dft";
    rp=ini.GetString("mem_cache_replacement","resource",rp);
    if(s_str2replacementUmap.contains(rp))
      replacement[RESOURCE_TYPE_RESOURCE]=s_str2replacementUmap[rp];
    rp="dft";
    rp=ini.GetString("mem_cache_replacement","user",rp);
    if(s_str2replacementUmap.contains(rp))
      replacement[RESOURCE_TYPE_USER]=s_str2replacementUmap[rp];
    rp="dft";
    rp=ini.GetString("mem_cache_replacement","name",rp);
    if(s_str2replacementUmap.contains(rp))
      replacement[RESOURCE_TYPE_NAME]=s_str2replacementUmap[rp];
    rp="dft";
    rp=ini.GetString("mem_cache_replacement","file",rp);
    if(s_str2replacementUmap.contains(rp))
      replacement[RESOURCE_TYPE_FILE]=s_str2replacementUmap[rp];
    rp="dft";
    rp=ini.GetString("mem_cache_replacement","passwd",rp);
    if(s_str2replacementUmap.contains(rp))
      replacement[RESOURCE_TYPE_PASSWD]=s_str2replacementUmap[rp];
    rp="dft";
    rp=ini.GetString("mem_cache_replacement","post",rp);
    if(s_str2replacementUmap.contains(rp))
      replacement[RESOURCE_TYPE_POST]=s_str2replacementUmap[rp];
    rp="dft";
    rp=ini.GetString("mem_cache_replacement","post_content",rp);
    if(s_str2replacementUmap.contains(rp))
      replacement[RESOURCE_TYPE_POST_CONTENT]=s_str2replacementUmap[rp];
  }
  m_dbMemCache=new DbMemCache(cacheSizeLimit,replacement);
  unordered_map<string,string>properties;
  {
    string pr;
    pr="";
    pr=ini.GetString("pool","hostName",pr);
    if(pr!="")
      properties["hostName"]=pr;
    pr="";
    pr=ini.GetString("pool","userName",pr);
    if(pr!="")
      properties["userName"]=pr;
    pr="";
    pr=ini.GetString("pool","password",pr);
    if(pr!="")
      properties["password"]=pr;
    pr="";
    pr=ini.GetString("pool","schema",pr);
    if(pr!="")
      properties["schema"]=pr;
    pr="";
    pr=ini.GetString("pool","port",pr);
    if(pr!="")
      properties["port"]=pr;
    pr="";
    pr=ini.GetString("pool","OPT_RECONNECT",pr);
    if(pr!="")
      properties["OPT_RECONNECT"]=pr;
    pr="";
    pr=ini.GetString("pool","cacheNum",pr);
    if(pr!="")
      properties["cacheNum"]=pr;
    pr="";
    pr=ini.GetString("pool","maxNum",pr);
    if(pr!="")
      properties["maxNum"]=pr;
  }
  m_dbPool=new MariadbPool(properties);
  ScopeConn sconn(m_dbPool);
  auto conn=sconn.get();
  {//max_allowed_packet
    PreparedStatement*stmnt(conn->prepareStatement(SQLString("show variables like 'max_allowed_packet'")));
    ResultSet*res=stmnt->executeQuery();
    res->next();
    m_maxAllowedPacket=res->getUInt64(2);
    delete res;
    delete stmnt;
  }
  //init resource_id bitmap
  memset(m_resourceIdBitMap,0,sizeof(m_resourceIdBitMap));
  const unsigned int k_limit=m_maxAllowedPacket/sizeof(resource_id_t);
  resource_id_t count=0;
  {
    PreparedStatement*stmnt(conn->prepareStatement(SQLString("select count(*) from `elevencent`.`resource`")));
    ResultSet*res=stmnt->executeQuery();
    if(res->next())
      count=res->getUInt64(1);
    delete res;
    delete stmnt;
  }
  if(count<=k_limit){
    PreparedStatement*stmnt(conn->prepareStatement(SQLString("select `resource_id` from `elevencent`.`resource`")));
    ResultSet*res=stmnt->executeQuery();
    while(res->next())
      setResourceIdBitMap(res->getUInt64(1));
    delete res;
    delete stmnt;
  }else{
    resource_id_t minResourceId=RESOURCE_ID_RANGE_MIN;
    while(count){
      PreparedStatement*stmnt(conn->prepareStatement(SQLString("select `resource_id` from `elevencent`.`resource` where `resource_id` >= "+to_string(minResourceId)+" limit "+to_string(k_limit))));
      ResultSet*res=stmnt->executeQuery();
      while(res->next()){
	--count;
	setResourceIdBitMap(minResourceId=res->getUInt64(1));
      }
      delete res;
      delete stmnt;
      ++minResourceId;
    }
  }
  for(m_resourceIdFreeStart=RESOURCE_ID_RANGE_MIN;isSetResourceIdBitMap(m_resourceIdFreeStart)&&m_resourceIdFreeStart<RESOURCE_ID_RANGE_MAX;++m_resourceIdFreeStart);
  pthread_mutex_init(&m_mutex,0);
}
void Resource::putConn(sql::Connection**pconn){
  m_dbPool->put(pconn);
}
bool Resource::peekFreeResourceId(resource_id_t*resourceId){
  if(!isSetResourceIdBitMap(m_resourceIdFreeStart)){
    if(resourceId)
      *resourceId=m_resourceIdFreeStart;
    return true;
  }
  resource_id_t resourceIdFreeStart=m_resourceIdFreeStart,id=m_resourceIdFreeStart;
  RESOURCE_ID_LOOP_INCRE(id);
  for(;id!=resourceIdFreeStart;){
    if(!isSetResourceIdBitMap(id)){
      if(resourceId)
	*resourceId=id;
      return true;
    }
    RESOURCE_ID_LOOP_INCRE(id);    
  }
  return false;
}
bool Resource::consumeFreeResourceId(resource_id_t*resourceId){
  if(!isSetResourceIdBitMap(m_resourceIdFreeStart)){
    setResourceIdBitMap(m_resourceIdFreeStart);
    if(resourceId)
      *resourceId=m_resourceIdFreeStart;
    RESOURCE_ID_LOOP_INCRE(m_resourceIdFreeStart);    
    return true;
  }
  resource_id_t resourceIdFreeStart=m_resourceIdFreeStart;
  RESOURCE_ID_LOOP_INCRE(m_resourceIdFreeStart);
  for(resource_id_t id=m_resourceIdFreeStart;id!=resourceIdFreeStart;){
    if(!isSetResourceIdBitMap(id)){
      setResourceIdBitMap(id);
      if(resourceId)
	*resourceId=id;
      m_resourceIdFreeStart=id;
      RESOURCE_ID_LOOP_INCRE(m_resourceIdFreeStart);
      return true;
    }
    RESOURCE_ID_LOOP_INCRE(id);    
  }
  return false;
}
bool Resource::consumeResourceId(resource_id_t resourceId){
  setResourceIdBitMap(resourceId);
  if(resourceId!=m_resourceIdFreeStart)
    return true;
  resource_id_t id=m_resourceIdFreeStart;
  RESOURCE_ID_LOOP_INCRE(m_resourceIdFreeStart);    
  while(m_resourceIdFreeStart!=id){
    if(!isSetResourceIdBitMap(m_resourceIdFreeStart))
      return true;
    RESOURCE_ID_LOOP_INCRE(m_resourceIdFreeStart);          
  }
  return false;
}
Connection**Resource::getConn(bool autoCommit){
  auto pconn=m_dbPool->get();
  auto conn=*pconn;
  conn->setAutoCommit(autoCommit);  
  return pconn;
}
void Resource::setResourceIdBitMap(resource_id_t resourceId){
  m_resourceIdBitMap[resourceId/8]|=(((resource_id_t)1)<<(resourceId%8));
}
void Resource::unSetResourceIdBitMap(resource_id_t resourceId){
  m_resourceIdBitMap[resourceId/8]&=(~(((resource_id_t)1)<<(resourceId%8)));
}
bool Resource::isSetResourceIdBitMap(resource_id_t resourceId){
  return m_resourceIdBitMap[resourceId/8]&(((resource_id_t)1)<<(resourceId%8));
}
Resource::~Resource(){
  pthread_mutex_destroy(&m_mutex);  
  delete m_dbPool;
  delete m_dbMemCache;
}
Resource::ERRNO Resource::selectNameResource(std::string name,resource_id_t*nId){
  if(name.size()>g_sizeLimit["name_resource_name"])
    return ERR_LIMIT_SIZE;  
  if(m_dbMemCache->getNameResource(name,nId))
    return OK;
  ScopeConn sconn(m_dbPool);
  auto conn=sconn.get();
  PreparedStatement*stmnt(conn->prepareStatement(SQLString("select `resource_id` from `elevencent`.`name_resource` where `name`=?")));
  stmnt->setString(1,name);
  ResultSet*res=stmnt->executeQuery();
  if(res->next()){
    if(nId)
      *nId=res->getUInt64(1);
    m_dbMemCache->updateResource(DbMapper::NameResource(res->getUInt64(1),name));
    delete res;
    delete stmnt;
    return OK;
  }
  delete res;	
  delete stmnt;
  return ERR_DB;
}
Resource::ERRNO Resource::selectPasswdResource(std::string pwd,resource_id_t*pId,resource_mask_t passwdMask){
  if(!(passwdMask&RESOURCE_MASK_PASSWD_PLAIN))
    return ERR_RESOURCE_MASK;
  if(pwd.size()>g_sizeLimit["passwd_resource_passwd"])
    return ERR_LIMIT_SIZE;
  if(m_dbMemCache->getPasswdResource(pwd,passwdMask,pId))
    return OK;
  ScopeConn sconn(m_dbPool);
  auto conn=sconn.get();
  PreparedStatement*stmnt(conn->prepareStatement(SQLString("select `resource_id` from `elevencent`.`passwd_resource` where `passwd`=? and `resource_mask`=?")));
  stmnt->setString(1,pwd);
  stmnt->setUInt64(2,passwdMask);
  ResultSet*res=stmnt->executeQuery();
  if(res->next()){
    if(pId)
      *pId=res->getUInt64(1);
    m_dbMemCache->updateResource(DbMapper::PasswdResource(res->getUInt64(1),passwdMask,pwd));
    delete res;
    delete stmnt;
    return OK;
  }
  delete res;	
  delete stmnt;
  return ERR_DB;
}
Resource::ERRNO Resource::selectUserResourceByNameId(resource_id_t*uId,resource_id_t id){
  if(m_dbMemCache->getUserResourceByNameId(uId,id))
    return OK;
  ScopeConn sconn(m_dbPool);
  auto conn=sconn.get();
  PreparedStatement*stmnt(conn->prepareStatement(SQLString("select `user_resource_id`,`resource_mask` from `elevencent`.`user_resource` where `resource_id`=?")));
  stmnt->setUInt64(1,id);
  ResultSet*res=stmnt->executeQuery();
  if(res->next()){
    if(uId)
      *uId=res->getUInt64(1);
    m_dbMemCache->updateResource(DbMapper::UserResource(res->getUInt64(1),id,res->getUInt64(2),0,0));    
    delete res;
    delete stmnt;
    return OK;
  }
  delete res;	
  delete stmnt;
  return ERR_DB;  
}
Resource::ERRNO Resource::isUserRefResource(resource_id_t uId,resource_id_t id){
  if(m_dbMemCache->getUserResource(uId,id))
    return OK;
  ScopeConn sconn(m_dbPool);
  auto conn=sconn.get();
  PreparedStatement*stmnt(conn->prepareStatement(SQLString("select `resource_mask` from `elevencent`.`user_resource` where `resource_id`=? and `user_resource_id`=?")));
  stmnt->setUInt64(1,id);
  stmnt->setUInt64(2,uId);  
  ResultSet*res=stmnt->executeQuery();
  if(res->next()){
    m_dbMemCache->updateResource(DbMapper::UserResource(uId,id,res->getUInt64(1),0,0));    
    delete res;
    delete stmnt;
    return OK;
  }
  delete res;	
  delete stmnt;
  return ERR_DB;  
}
Resource::ERRNO Resource::insertNameResource(string name,resource_id_t*nameId,resource_mask_t resourceMask,DB_MEM_CACHE_TYPE type){
  if(name.size()>g_sizeLimit["name_resource_name"])
    return ERR_LIMIT_SIZE;  
  if(m_dbMemCache->getNameResource(name,nameId))
    return WARN_RESOURCE_EXIST;
  switch(type){
  case DB_MEM_CACHE_TYPE_THROUGH:{
    ScopeConn sconn(m_dbPool);
    auto conn=sconn.get(false);
    {
      PreparedStatement*stmnt(conn->prepareStatement(SQLString("select `resource_id` from `elevencent`.`name_resource` where `name`=?")));
      stmnt->setString(1,name);
      ResultSet*res=stmnt->executeQuery();
      conn->commit();
      if(res->next()){
	if(nameId)
	  *nameId=res->getUInt64(1);
	delete res;
	delete stmnt;
	return WARN_RESOURCE_EXIST;
      }
      delete res;	
      delete stmnt;	
    }
    resource_id_t nId;
    if(!peekFreeResourceId(&nId))
      return ERR_RESOURCE_ID;
    {
      PreparedStatement*stmnt(conn->prepareStatement(SQLString("insert into `elevencent`.`resource` (`resource_id`,`resource_mask`,`resource_ref`) values (?,?,0)")));
      stmnt->setUInt64(1,nId);
      stmnt->setUInt64(2,resourceMask);
      if(stmnt->executeUpdate()!=1){
	conn->rollback();
	delete stmnt;
	return ERR_DB;
      }
      delete stmnt;
    }      
    {
      PreparedStatement*stmnt(conn->prepareStatement(SQLString("insert into `elevencent`.`name_resource` (`resource_id`,`name`) values (?,?)")));
      stmnt->setUInt64(1,nId);
      stmnt->setString(2,name);
      if(stmnt->executeUpdate()!=1){
	conn->rollback();
	delete stmnt;
	return ERR_DB;
      }
      delete stmnt;
    }    
    conn->commit();
    consumeResourceId(nId);
    if(nameId)
      *nameId=nId;
    m_dbMemCache->updateResource(DbMapper::NameResource(nId,name));
    m_dbMemCache->updateResource(DbMapper::Resource(nId,resourceMask,0));
    return ERRNO::OK;
  }
    break;
  default:
    break;
  }
  return ERR_COMM;
}
Resource::ERRNO Resource::insertPasswdResource(string passwd,resource_id_t*passwdId,resource_mask_t passwdMask,resource_mask_t resourceMask,DB_MEM_CACHE_TYPE type){
  switch(type){
  case DB_MEM_CACHE_TYPE_THROUGH:{
    if(passwdMask&RESOURCE_MASK_PASSWD_PLAIN){
      if(passwd.size()>g_sizeLimit["passwd_resource_passwd"])
	return ERR_LIMIT_SIZE;
      if(m_dbMemCache->getPasswdResource(passwd,passwdMask,passwdId))
	return WARN_RESOURCE_EXIST;
      ScopeConn sconn(m_dbPool);
      auto conn=sconn.get(false);
      {
	PreparedStatement*stmnt(conn->prepareStatement(SQLString("select `resource_id` from `elevencent`.`passwd_resource` where `passwd`=? and `resource_mask`=?")));
	stmnt->setString(1,passwd);
	stmnt->setUInt64(2,passwdMask);
	ResultSet*res=stmnt->executeQuery();
	conn->commit();
	if(res->next()){
	  if(passwdId)
	    *passwdId=res->getUInt64(1);
	  delete res;
	  delete stmnt;
	  return WARN_RESOURCE_EXIST;
	}
	delete res;	
	delete stmnt;	
      }
      resource_id_t pId;
      if(!peekFreeResourceId(&pId))
	return ERR_RESOURCE_ID;      
      {
	PreparedStatement*stmnt(conn->prepareStatement(SQLString("insert into `elevencent`.`resource` (`resource_id`,`resource_mask`,`resource_ref`) values (?,?,0)")));
	stmnt->setUInt64(1,pId);
	stmnt->setUInt64(2,resourceMask);
	if(stmnt->executeUpdate()!=1){
	  conn->rollback();
	  delete stmnt;
	  return ERR_DB;
	}
	delete stmnt;
      }      
      {
	PreparedStatement*stmnt(conn->prepareStatement(SQLString("insert into `elevencent`.`passwd_resource` (`resource_id`,`passwd`,`resource_mask`) values (?,?,?)")));
	stmnt->setUInt64(1,pId);
	stmnt->setString(2,passwd);
	stmnt->setUInt64(3,passwdMask);
	if(stmnt->executeUpdate()!=1){
	  conn->rollback();
	  delete stmnt;
	  return ERR_DB;
	}
	delete stmnt;
      }    
      conn->commit();
      consumeResourceId(pId);
      if(passwdId)
	*passwdId=pId;
      m_dbMemCache->updateResource(DbMapper::PasswdResource(pId,passwdMask,passwd));
      m_dbMemCache->updateResource(DbMapper::Resource(pId,resourceMask,0));
      return ERRNO::OK;
    }
  }
    break;
  default:
    return ERR_COMM;
  }
  return ERR_COMM;
}
Resource::ERRNO Resource::insertPostContentResource(resource_id_t*id,void*buf,size_t len,resource_mask_t mask,resource_mask_t resourceMask,DB_MEM_CACHE_TYPE type){
  if(len>g_sizeLimit["post_content_resource_post_content"])
    return ERR_LIMIT_SIZE;
  resource_id_t pId;
  if(!peekFreeResourceId(&pId))
    return ERR_RESOURCE_ID;
  if(id)
    *id=pId;
  switch(type){
  case DB_MEM_CACHE_TYPE_THROUGH:{
    ScopeConn sconn(m_dbPool);
    auto conn=sconn.get(false);
    {
      PreparedStatement*stmnt(conn->prepareStatement(SQLString("insert into `elevencent`.`resource` (`resource_id`,`resource_mask`,`resource_ref`) values (?,?,0)")));
      stmnt->setUInt64(1,pId);
      stmnt->setUInt64(2,resourceMask);
      if(stmnt->executeUpdate()!=1){
	conn->rollback();
	delete stmnt;
	return ERR_DB;
      }
      delete stmnt;
    }
    {
      PreparedStatement*stmnt(conn->prepareStatement(SQLString("insert into `elevencent`.`post_content_resource` (`resource_id`,`post_content`,`resource_mask`) values (?,?,?)")));
      stmnt->setUInt64(1,pId);
      IStreamBlobBuf ibuf((char*)buf,len);
      istream is(&ibuf);
      stmnt->setBlob(2,&is);
      stmnt->setUInt64(3,mask);
      if(stmnt->executeUpdate()!=1){
	conn->rollback();
	delete stmnt;
	return ERR_DB;
      }
      delete stmnt;
    }
    conn->commit();
    consumeResourceId(pId);
    m_dbMemCache->updateResource(DbMapper::Resource(pId,mask,0));
    return OK;
  }
    break;
  default:
    break;
  }
  return ERR_COMM;
}
Resource::ERRNO Resource::selectPostContentResource(resource_id_t uId,resource_id_t nId,datetime_t from,datetime_t to,ResultSet**res){
  ScopeConn sconn(m_dbPool);
  auto conn=sconn.get();
  PreparedStatement*stmnt(conn->prepareStatement(SQLString("select `elevencent`.`user_resource`.`user_resource_id` as user_resource_user_resource_id,`elevencent`.`user_resource`.`resource_id` as user_resource_resource_id,`elevencent`.`user_resource`.`resource_mask` as user_resource_resource_mask,UNIX_TIMESTAMP(`elevencent`.`user_resource`.`create_time`)*1000000 as user_resource_create_time,UNIX_TIMESTAMP(`elevencent`.`user_resource`.`update_time`)*1000000 as user_resource_update_time,`elevencent`.`post_content_resource`.`post_content` as post_content_resource_post_content,`elevencent`.`post_content_resource`.`resource_mask` as post_content_resource_resource_mask from `elevencent`.`user_resource` inner join `elevencent`.`post_content_resource` on `elevencent`.`user_resource`.`resource_id`=`elevencent`.`post_content_resource`.`resource_id`  where `elevencent`.`user_resource`.`resource_id` in (select `resource_id` from `elevencent`.`user_resource` where `user_resource_id`= ? and resource_mask & ? ) and `elevencent`.`user_resource`.`resource_id` in (select `resource_id` from `elevencent`.`user_resource` where `user_resource_id`= ? and resource_mask & ? ) and `elevencent`.`user_resource`.`user_resource_id`=? and UNIX_TIMESTAMP(update_time)*1000000 between ? and ? order by `elevencent`.`user_resource`.`update_time` asc;"),ResultSet::TYPE_FORWARD_ONLY,ResultSet::CONCUR_READ_ONLY));
  stmnt->setUInt64(1,nId);
  stmnt->setUInt64(2,RESOURCE_MASK_USER_POST_SENDER|RESOURCE_MASK_USER_POST_RECVER);
  stmnt->setUInt64(3,uId);
  stmnt->setUInt64(4,RESOURCE_MASK_USER_POST_SENDER|RESOURCE_MASK_USER_POST_RECVER);
  stmnt->setUInt64(5,uId);  
  stmnt->setUInt64(6,from);
  stmnt->setUInt64(7,to);
  *res=stmnt->executeQuery();
  delete stmnt;
  return OK;
}
Resource::ERRNO Resource::selectMsgUsers(resource_id_t uId,ResultSet**res){
  ScopeConn sconn(m_dbPool);
  auto conn=sconn.get();
  PreparedStatement*stmnt(conn->prepareStatement(SQLString("select name from `elevencent`.`name_resource` where resource_id in ( select distinct resource_id from `elevencent`.`resource` where resource_mask & ? and resource_id in (  select resource_id from `elevencent`.`user_resource` where user_resource_id in (select distinct user_resource_id from `elevencent`.`user_resource` where resource_id in (select resource_id from `elevencent`.`user_resource` where user_resource_id=? and resource_mask & ? ))));"),ResultSet::TYPE_FORWARD_ONLY,ResultSet::CONCUR_READ_ONLY));
  stmnt->setUInt64(1,RESOURCE_MASK_RESOURCE_NAME);
  stmnt->setUInt64(2,uId);  
  stmnt->setUInt64(3,RESOURCE_MASK_USER_POST_SENDER|RESOURCE_MASK_USER_POST_RECVER);
  *res=stmnt->executeQuery();
  delete stmnt;
  return OK;
}
Resource::ERRNO Resource::insertUserResource(resource_id_t*userResourceId,resource_mask_t userResourceMask,resource_mask_t resourceMask,DB_MEM_CACHE_TYPE type){
  resource_id_t uId;
  if(!peekFreeResourceId(&uId))
    return ERR_RESOURCE_ID;  
  switch(type){
  case DB_MEM_CACHE_TYPE_THROUGH:{
    ScopeConn sconn(m_dbPool);
    auto conn=sconn.get(false);
    {
      PreparedStatement*stmnt(conn->prepareStatement(SQLString("insert into `elevencent`.`resource` (`resource_id`,`resource_mask`,`resource_ref`) values (?,?,1)")));
      stmnt->setUInt64(1,uId);
      stmnt->setUInt64(2,resourceMask);
      if(stmnt->executeUpdate()!=1){
	conn->rollback();
	delete stmnt;
	return ERR_DB;
      }
      delete stmnt;
    }
    {
      PreparedStatement*stmnt(conn->prepareStatement(SQLString("insert into `elevencent`.`user_resource` (`user_resource_id`,`resource_id`,`resource_mask`) values (?,?,?)")));
      stmnt->setUInt64(1,uId);
      stmnt->setUInt64(2,uId);
      stmnt->setUInt64(3,userResourceMask);
      if(stmnt->executeUpdate()!=1){
	conn->rollback();
	delete stmnt;
	return ERR_DB;
      }
      delete stmnt;
    }    
    conn->commit();
    consumeResourceId(uId);
    if(userResourceId)
      *userResourceId=uId;
    m_dbMemCache->updateResource(DbMapper::UserResource(uId,uId,userResourceMask,0,0));
    m_dbMemCache->updateResource(DbMapper::Resource(uId,resourceMask,1));
    return ERRNO::OK;
  }
    break;
  default:
    break;
  }
  return ERR_COMM;
}
Resource::ERRNO Resource::userRefResource(resource_id_t userResourceId,resource_id_t resourceId,resource_mask_t resourceMask,DB_MEM_CACHE_TYPE type){
  if(!isSetResourceIdBitMap(userResourceId)||!isSetResourceIdBitMap(resourceId))
    return ERR_RESOURCE_ID;
  switch(type){
  case DB_MEM_CACHE_TYPE_THROUGH:{
    if(m_dbMemCache->getUserResource(userResourceId,resourceId))
      return WARN_RESOURCE_EXIST;
    ScopeConn sconn(m_dbPool);
    auto conn=sconn.get(false);
    {
      PreparedStatement*stmnt(conn->prepareStatement(SQLString("select `resource_mask`,`resource_ref` from `elevencent`.`resource` where `resource_id`=?")));
      stmnt->setUInt64(1,userResourceId);
      ResultSet*res=stmnt->executeQuery();
      conn->commit();
      if(!res->next()){
	delete res;
	delete stmnt;
	return ERR_RESOURCE_ID;
      }
      resource_mask_t mask=res->getUInt64(1);
      uint64_t ref=res->getUInt64(2);
      if(!(mask&RESOURCE_MASK_RESOURCE_USER)){
	delete res;
	delete stmnt;
	return ERR_RESOURCE_TYPE;
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
	return WARN_RESOURCE_EXIST;
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
	return ERR_DB;
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
	return ERR_DB;
      }
      delete stmnt;      
    }
    conn->commit();
    auto resourceOpt=m_dbMemCache->getResource(resourceId);
    if(resourceOpt)
      m_dbMemCache->updateResource(DbMapper::Resource(resourceId,resourceOpt->m_resourceMask,resourceOpt->m_resourceRef+1));
    m_dbMemCache->updateResource(DbMapper::UserResource(userResourceId,resourceId,resourceMask,0,0));
    return OK;
  }
    break;
  default:
    break;
  }
  return ERR_COMM;
}
bool Resource::isErr(ERRNO err){
  for(auto i=ERR_START<<1;i!=ERR_END;i<<=1)
    if(err&i)
      return true;
  return false;
}
bool Resource::isWarn(ERRNO err){
  for(auto i=WARN_START<<1;i!=WARN_END;i<<=1)
    if(err&i)
      return true;
  return false;
}
bool Resource::isOk(ERRNO err){
  return err==OK;
}
