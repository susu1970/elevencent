#include"mariadb_pool.h"
#include"global.h"

using namespace std;
using namespace elevencent;
using namespace sql;

#define CACHED_NUM_DFT g_processNum
#define MAX_NUM_DFT MAX(g_processNum,CACHED_NUM_DFT)

#define LOCK_IDLE pthread_mutex_lock(&m_idleMutex)
#define UNLOCK_IDLE pthread_mutex_unlock(&m_idleMutex)
#define LOCK_BUSY pthread_mutex_lock(&m_busyMutex)
#define UNLOCK_BUSY pthread_mutex_unlock(&m_busyMutex)

#define HAS_CONN(umap,conn) (umap.find(conn)!=umap.end())

MariadbPool::MariadbPool(std::unordered_map<std::string,std::string>propertiesUMap){
  m_driver=mariadb::get_driver_instance();
  for(auto iter=propertiesUMap.begin();iter!=propertiesUMap.end();++iter)
    m_connProperties[iter->first]=iter->second;
  m_cacheNum=CACHED_NUM_DFT;
  m_maxNum=MAX_NUM_DFT;
  if(propertiesUMap.find(MARIADB_POOL_CACHE_NUM_KEY)!=propertiesUMap.end())
    m_cacheNum=stoi(propertiesUMap[MARIADB_POOL_CACHE_NUM_KEY]);
  if(propertiesUMap.find(MARIADB_POOL_MAX_NUM_KEY)!=propertiesUMap.end())
    m_cacheNum=stoi(propertiesUMap[MARIADB_POOL_MAX_NUM_KEY]);
  if(m_cacheNum<=0)
    m_cacheNum=CACHED_NUM_DFT;
  if(m_maxNum<m_cacheNum)
    m_maxNum=m_cacheNum;
  pthread_mutex_init(&m_idleMutex,0);
  pthread_mutex_init(&m_busyMutex,0);
}

void MariadbPool::createConn(short num){
  LOCK_IDLE;
  LOCK_BUSY;
  num=getMaxD(m_idleUMap.size()+m_busyUMap.size());
  UNLOCK_BUSY;
  try{
    while(num-->0)
      m_idleUMap[m_driver->connect(m_connProperties)]=true;
  }catch(SQLException&e){
    DEBUG_MSG("Connect to mariadb error:\n"<<e.what());
  }
  UNLOCK_IDLE;
}

short MariadbPool::getMaxD(short curNum){
  short cacheD=m_cacheNum-curNum;
  short maxD=m_maxNum-curNum;
  return MIN(cacheD,maxD);
}

Connection*MariadbPool::getConnection(){
  Connection*ret=0;
  LOCK_IDLE;
  if(!m_idleUMap.empty()){
    ret=m_idleUMap.begin()->first;
    m_idleUMap.erase(ret);
    LOCK_BUSY;    
    m_busyUMap[ret]=true;
    UNLOCK_IDLE;        
    UNLOCK_BUSY;    
    return ret;
  }
  LOCK_BUSY;
  short num=getMaxD(m_busyUMap.size());
  if(num>0){
    try{
      m_busyUMap[ret=m_driver->connect(m_connProperties)]=true;
    }catch(SQLException&e){
      DEBUG_ASSERT(0,"Connect to mariadb error:\n"<<e.what());
    }
    UNLOCK_BUSY;
    while(--num>0){
      try{
	m_idleUMap[m_driver->connect(m_connProperties)]=true;
      }catch(SQLException&e){
	DEBUG_MSG("Connect to mariadb error:\n"<<e.what());
      }
    }
    UNLOCK_IDLE;
    return ret;
  }
  UNLOCK_IDLE;        
  UNLOCK_BUSY;    
  return 0;
}

void MariadbPool::recycleConnection(Connection*conn){
  if(!conn)return;
  LOCK_IDLE;
  LOCK_BUSY;
  if(!HAS_CONN(m_busyUMap,conn)){
    UNLOCK_IDLE;
    UNLOCK_BUSY;
    return;
  }
  if(m_idleUMap.empty()){
    m_busyUMap.erase(conn);
    m_idleUMap[conn]=true;
    UNLOCK_IDLE;
    UNLOCK_BUSY;
    return;
  }
  short curNum=m_busyUMap.size()+m_idleUMap.size();
  short cacheD=m_cacheNum-curNum;
  if(cacheD<0){
    conn->close();
    delete conn;
    UNLOCK_IDLE;
    UNLOCK_BUSY;
    return;
  }
  m_busyUMap.erase(conn);
  m_idleUMap[conn]=true;
  UNLOCK_IDLE;
  UNLOCK_BUSY;
  return;
}

MariadbPool::~MariadbPool(){
  

  pthread_mutex_destroy(&m_idleMutex);
  pthread_mutex_destroy(&m_busyMutex);
}
