#include"mariadb_pool.h"
#include"global.h"

using namespace std;
using namespace elevencent;
using namespace sql;

MariadbPool::MariadbPool(std::unordered_map<std::string,std::string>propertiesUMap){
  m_driver=mariadb::get_driver_instance();
  uint16_t cacheNum=g_processNum;
  if(propertiesUMap.find("cacheNum")!=propertiesUMap.end())
    cacheNum=stoi(propertiesUMap["cacheNum"]);
  for(auto iter=propertiesUMap.begin();iter!=propertiesUMap.end();++iter)
    if(iter->first!="cacheNum")
      m_connProperties[iter->first]=iter->second; 
  m_head=m_tail=new Node;
  for(uint16_t i=0;i<cacheNum;++i){
    Node*p=new Node;
    try{
      p->conn=m_driver->connect(m_connProperties);
    }catch(SQLException&e){
      DEBUG_MSG("Connect to mariadb error:\n"<<e.what());
      exit(-1);
    }
    put((Connection**)p);
  }
}
Connection**MariadbPool::get(bool wait){
  Node*p;
  while(!(p=m_head->next)){
    if(!wait)
      return 0;
    sched_yield();
  }
  while(!CAS_BOOL((uint64_t*)(&m_head->next),(uint64_t)p,(uint64_t)p->next)){
    while(!(p=m_head->next)){
      if(!wait)
	return 0;
      sched_yield();
    }
    sched_yield();    
  }
  return &p->conn;
}
void MariadbPool::put(Connection**conn){
  Node*p=(Node*)conn;
  p->next=0;
  while(!CAS_BOOL((uint64_t*)&m_tail->next,0,(uint64_t)p))
    sched_yield();
  m_tail=p;
}
MariadbPool::~MariadbPool(){
  for(auto p=m_head;p;p=p->next){
    if(p->conn){
      p->conn->close();
      delete p->conn;
    }
    delete p;
  }
}
