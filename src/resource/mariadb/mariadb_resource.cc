#include"mariadb_resource.h"

using namespace std;
using namespace elevencent;

MariadbResource::MariadbResource(ThreadPool&&thrPool,MariadbPool&&m_dbPool):m_thrPool(forward<ThreadPool>(thrPool)),m_dbPool(forward<MariadbPool>(m_dbPool)){
  
}
