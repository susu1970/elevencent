#ifndef ELEVENCENT_MARIADB_POOL_H_
#define ELEVENCENT_MARIADB_POOL_H_

#include<unordered_map>
#include<pthread.h>
#include<mariadb/conncpp.hpp>

namespace elevencent{
#define MARIADB_POOL_CACHE_NUM_KEY "cacheNum"
#define MARIADB_POOL_MAX_NUM_KEY "maxNum"
  class MariadbPool{
    std::unordered_map<sql::Connection*,bool>m_idleUMap,m_busyUMap;
    sql::ConnectOptionsMap m_connProperties;
    short m_cacheNum,m_maxNum;
    sql::Driver*m_driver;
  private:
    sql::Connection*getConnection();
    void recycleConnection(sql::Connection*conn);
  public:
    MariadbPool(std::unordered_map<std::string,std::string>propertiesUMap);    
    static sql::Connection*getConn(MariadbPool*pool);
    static void recycleConn(MariadbPool*pool,sql::Connection*conn);
    static void test();
    ~MariadbPool();    
  };
}

#endif
