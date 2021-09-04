#ifndef _MARIADB_RESOURCE_H_
#define _MARIADB_RESOURCE_H_

#include"resource.h"
#include"mariadb_pool.h"
#include"thread_pool.h"

namespace elevencent{
#define MAX_MARIADB_PACKET 1073741824//more details on: https://mariadb.com/kb/en/server-system-variables/#max_allowed_packet.Moreover,we limit that each packet send to mariadb has MAX_ALLOWED_PACKET bytes at most.
  class MariadbResource:public Resource{
    MariadbPool m_dbPool;
    ThreadPool m_thrPool;
  public:
    MariadbResource(ThreadPool&&thrPool,MariadbPool&&m_dbPool);
    MariadbResource()
  };
}


#endif /* _MARIADB_RESOURCE_H_ */
