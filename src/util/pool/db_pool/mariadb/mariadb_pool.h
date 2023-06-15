#ifndef ELEVENCENT_MARIADB_POOL_H_
#define ELEVENCENT_MARIADB_POOL_H_

#include<unordered_map>
#include<mariadb/conncpp.hpp>
#include"global.h"

namespace elevencent{
  class MariadbPool{
    class Node{
      friend MariadbPool;
      sql::Connection*conn=0;
      Node*next=0;
    };
    volatile Node*m_head,*m_tail;
    sql::ConnectOptionsMap m_connProperties;
    sql::Driver*m_driver;
  public:
    MariadbPool(std::unordered_map<std::string,std::string>propertiesUMap);
    sql::Connection**get(bool wait=true);
    void put(sql::Connection**);
    ~MariadbPool();
    class ScopeConn{
      MariadbPool*pool;
      sql::Connection**pconn=0;
      sql::Connection*conn=0;      
    public:
      ScopeConn(MariadbPool*pool_):pool(pool_){
	pconn=pool->get();
	conn=*pconn;
      }
      sql::Connection*get(bool autoCommit=true){
	conn->setAutoCommit(autoCommit);	
	return conn;
      }
      ~ScopeConn(){
	pool->put(pconn);
      }
    };
  };
}

#endif
