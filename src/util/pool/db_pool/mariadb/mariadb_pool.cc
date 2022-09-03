
#include"mariadb_pool.h"
#include"global.h"

using namespace std;
using namespace elevencent;
using namespace sql;

static short g_cacheNumDft=g_processNum;
static short g_maxNumDft=MAX(g_processNum,g_cacheNumDft);

MariadbPool::MariadbPool(std::unordered_map<std::string,std::string>propertiesUMap){
  m_driver=mariadb::get_driver_instance();
  m_cacheNum=g_cacheNumDft;
  m_maxNum=g_maxNumDft;
  if(propertiesUMap.find(MARIADB_POOL_CACHE_NUM_KEY)!=propertiesUMap.end())
    m_cacheNum=stoi(propertiesUMap[MARIADB_POOL_CACHE_NUM_KEY]);
  if(propertiesUMap.find(MARIADB_POOL_MAX_NUM_KEY)!=propertiesUMap.end())
    m_maxNum=stoi(propertiesUMap[MARIADB_POOL_MAX_NUM_KEY]);  
  for(auto iter=propertiesUMap.begin();iter!=propertiesUMap.end();++iter)
    if(iter->first!=MARIADB_POOL_CACHE_NUM_KEY&&iter->first!=MARIADB_POOL_MAX_NUM_KEY)
      m_connProperties[iter->first]=iter->second; 
  if(m_cacheNum<=0)
    m_cacheNum=g_cacheNumDft;
  if(m_maxNum<=0)
    m_maxNum=g_maxNumDft;
}

Connection*MariadbPool::getConnection(){
  Connection*ret;      
  if(!m_idleUMap.empty()){
    ret=m_idleUMap.begin()->first;
    m_idleUMap.erase(ret);
    m_busyUMap[ret]=true;
    return ret;
  }
  short curNum=m_busyUMap.size();
  short num=m_cacheNum-curNum;
  if(num<=0&&(num=m_maxNum-curNum)>1)
    num=1;
  if(num>0){
    try{
      m_busyUMap[ret=m_driver->connect(m_connProperties)]=true;
    }catch(SQLException&e){
      m_busyUMap.erase(ret);
      DEBUG_MSG("Connect to mariadb error:\n"<<e.what());      
      return 0;
    }
    Connection*tempConn;    
    while(--num>0){
      try{
	m_idleUMap[tempConn=m_driver->connect(m_connProperties)]=true;
      }catch(SQLException&e){
	m_idleUMap.erase(tempConn);
	DEBUG_MSG("Connect to mariadb error:\n"<<e.what());	
	return ret;		
      }
    }
    return ret;		
  }
  return 0;
}

void MariadbPool::recycleConnection(Connection*conn){
  if(m_busyUMap.find(conn)==m_busyUMap.end())
    return;
  if(m_idleUMap.empty()){
    m_busyUMap.erase(conn);
    m_idleUMap[conn]=true;
    return;
  }
  short curNum=m_busyUMap.size()+m_idleUMap.size();
  short cacheD=m_cacheNum-curNum;
  if(cacheD<0){
    m_busyUMap.erase(conn);
    conn->close();
    delete conn;
    return;
  }
  m_idleUMap[conn]=true;
  m_busyUMap.erase(conn);  
  return;
}

sql::Connection*MariadbPool::getConn(MariadbPool*pool){
  return pool?pool->getConnection():0;
}
void MariadbPool::recycleConn(MariadbPool*pool,sql::Connection*conn){
  if(!conn)return;
  if(!pool){
    conn->close();
    delete conn;
    return;
  }
  pool->recycleConnection(conn);
}


MariadbPool::~MariadbPool(){
  for(auto iter=m_idleUMap.begin();iter!=m_idleUMap.end();++iter){
    if(!iter->first)continue;
    ((Connection*)iter->first)->close();
    delete ((Connection*)iter->first);
  }
}
