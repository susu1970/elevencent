#ifndef ELEVENCENT_MARIADB_POOL_H_
#define ELEVENCENT_MARIADB_POOL_H_

#include<unordered_map>
#include<pthread.h>
#include<mariadb/conncpp.hpp>

namespace elevencent{
#define MARIADB_POOL_CACHE_NUM_KEY "poolCacheNum"
#define MARIADB_POOL_MAX_NUM_KEY "poolMaxNum"
  class MariadbPool{
    std::unordered_map<sql::Connection*,bool>m_idleUMap,m_busyUMap;
    sql::ConnectOptionsMap m_connProperties;
    short m_cacheNum,m_maxNum,m_curNum;
    pthread_mutex_t m_idleMutex,m_busyMutex;
    sql::Driver*m_driver;
  private:
    void createConn(short num=1);
    short getMaxD(short curNum);
  public:
    MariadbPool(std::unordered_map<std::string,std::string>propertiesUMap);
    sql::Connection*getConnection();
    void recycleConnection(sql::Connection*conn);
    ~MariadbPool();
  };
}

#endif
