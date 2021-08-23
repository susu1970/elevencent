#include "mariadb_elevencent"

using namespace std;
using namespace sql;
using namespace elevencent;
using namespace mariadb;

#define GET_PROPERTIES_VALUE(key) m_properties.find(key)==m_properties.end()?key:m_properties[key]

MariadbElevencent::MariadbElevencent(unordered_map<string,string>properties):m_properties(properties){}

bool MariadbElevencent::insertResource(Connection*conn,resource_id_t id,resource_mask_t mask){
  if(!conn
     ||HAS_RESOURCE_ID_RESERVED(id))
    return false;
  string sql="insert into `?` (`resource_id`,`resource_mask`,`resource_ref`) values (?,?,?)";
  try{
    PreparedStatement*stmnt(conn->prepareStatement(sql));
    smtnt->setString(1,GET_PROPERTIES_VALUE(RESOURCE_TABLE_NAME_KEY));
    stmnt->setUInt(2,id);
    stmnt->setUInt64(3,mask);
    stmnt->setUInt(4,0);    
    stmnt->executeUpdate();
    delete stmnt;
  }catch(SQLException&e){
    DEBUG_MSG("resource_id: "<<id<<"\nresource_mask: "<<mask<<"\ntable: "<<GET_PROPERTIES_VALUE(RESOURCE_TABLE_NAME_KEY)<<"\nSQLException: "<<e.what());
    return false;
  }
  return true;
}
bool MariadbElevencent::increResourceRef(Connection*conn,resource_id_t id){
  if(!conn
     ||HAS_RESOURCE_ID_RESERVED(id))
    return false;
  string sql="update `?` set `resource_ref`=`resource_ref`+1 where `resource_id`=?";
  try{
    PreparedStatement*stmnt(conn->prepareStatement(sql));
    stmnt->setString(1,GET_PROPERTIES_VALUE(RESOURCE_TABLE_NAME_KEY));
    stmnt->setUInt(2,id);
    stmnt->executeUpdate();
    delete stmnt;
  }catch(SQLException&e){
    DEBUG_MSG("resource_id: "<<id<<"\nSQLException: "<<e.what());
    return false;
  }
  return true;
}
bool MariadbElevencent::decreResourceRef(Connection*conn,resource_id_t id){
  if(!conn
     ||HAS_RESOURCE_ID_RESERVED(id))
    return false;
  string sql="select `resource_ref` from `?` where `resource_id`=?";
  resource_id_t ref;
  try{
    PreparedStatement*stmnt(conn->prepareStatement(sql));
    stmnt->setString(1,GET_PROPERTIES_VALUE(RESOURCE_TABLE_NAME_KEY));
    stmnt->setUInt(2,id);
    sql::ResultSet*res=stmnt->executeQuery();
    if(!res->next()){
      delete stmnt;
      return true;      
    }
    ref=res->getInt(1);
    delete stmnt;    
  }catch(SQLException&e){
    DEBUG_MSG("resource_id: "<<id<<"\nSQLException: "<<e.what());
    return false;
  }
  sql=ref<=1?"delete from `?` where `resource_id`=?":"update `?` set `resource_ref`=`resource_ref`-1 where `resource_id`=?";
  try{
    PreparedStatement*stmnt(conn->prepareStatement(sql));
    stmnt->setString(1,GET_PROPERTIES_VALUE(RESOURCE_TABLE_NAME_KEY));
    stmnt->setUInt(2,id);
    stmnt->executeUpdate();
    delete stmnt;
  }catch(SQLException&e){
    DEBUG_MSG("resource_id: "<<id<<"\nSQLException: "<<e.what());
    return false;
  }
  return true;
}
bool MariadbElevencent::insertPasswdResource(Connection*conn,resource_id_t id,resource_mask_t mask,string passwd){
  if(!conn
     ||HAS_RESOURCE_ID_RESERVED(id)
     ||passwd.size()>RESOURCE_PASSWD_MAX_LEN)
    return false;
  string sql="insert into `?` (`resource_id`,`resource_mask`,`passwd`) values (?,?,?)";
  try{
    PreparedStatement*stmnt(conn->prepareStatement(sql));
    stmnt->setString(1,GET_PROPERTIES_VALUE(PASSWD_RESOURCE_TABLE_NAME_KEY));
    stmnt->setUInt(2,id);
    stmnt->setUInt64(3,mask);
    stmnt->setString(4,passwd);
    stmnt->executeUpdate();    
    delete stmnt;
  }catch(SQLException&e){
    DEBUG_MSG("resource_id: "<<id<<"\nresource_mask: "<<mask<<"\npasswd: "<<passwd<<"\ntable: "<<GET_PROPERTIES_VALUE(PASSWD_RESOURCE_TABLE_NAME_KEY)<<"\nSQLException: "<<e.what());
    return false;
  }
  return true;
}
