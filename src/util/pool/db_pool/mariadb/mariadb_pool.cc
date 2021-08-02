
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
  m_hasWaitIdle=false;
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
  pthread_cond_init(&m_idleEmptyCond,0);
}

Connection*MariadbPool::getConnection(bool noWait){
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
  short curNum=m_busyUMap.size();
  short num=m_cacheNum-curNum;
  if(num<=0&&(num=m_maxNum-curNum)>1)
    num=1;
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
  UNLOCK_BUSY;    
  if(noWait){
    UNLOCK_IDLE;        
    return 0;
  }
  while(m_idleUMap.empty()){
    m_hasWaitIdle=true;
    pthread_cond_wait(&m_idleEmptyCond,&m_idleMutex);
  }
  m_hasWaitIdle=false;
  ret=m_idleUMap.begin()->first;
  m_idleUMap.erase(ret);
  LOCK_BUSY;
  m_busyUMap[ret]=true;
  UNLOCK_IDLE;
  UNLOCK_BUSY;
  return ret;
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
    if(m_hasWaitIdle)
      pthread_cond_signal(&m_idleEmptyCond);
    UNLOCK_IDLE;
    UNLOCK_BUSY;
    return;
  }
  short curNum=m_busyUMap.size()+m_idleUMap.size();
  short cacheD=m_cacheNum-curNum;
  if(cacheD<0){
    m_busyUMap.erase(conn);
    UNLOCK_IDLE;
    UNLOCK_BUSY;
    conn->close();
    delete conn;
    return;
  }
  m_idleUMap[conn]=true;
  UNLOCK_IDLE;
  m_busyUMap.erase(conn);  
  UNLOCK_BUSY;
  return;
}

MariadbPool::~MariadbPool(){
  

  pthread_mutex_destroy(&m_idleMutex);
  pthread_mutex_destroy(&m_busyMutex);
  pthread_cond_destroy(&m_idleEmptyCond);
}
