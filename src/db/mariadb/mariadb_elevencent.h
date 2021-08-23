#ifndef _MARIADB_ELEVENCENT_H_
#define _MARIADB_ELEVENCENT_H_

#include<string>
#include<mariadb/conncpp.hpp>
#include<unordered_map>
#include<hiredis/hiredis.h>

namespace elevencent{
  namespace mariadb{
#define MAX_ALLOWED_PACKET 1073741824//more details on: https://mariadb.com/kb/en/server-system-variables/#max_allowed_packet.Moreover,we limit that each column has MAX_ALLOWED_PACKET bytes at most.
    typedef unsigned int resource_id_t;
    typedef unsigned long resource_mask_t;
    
    enum class RESOURCE_TYPE:unsigned char{
      START,

      USER,
      NAME,
      RESOURCE,
      FILE,
      PASSWD,
      POST,        

      END,
    };

    enum class RESOURCE_ID_RESERVED:resource_id_t{
      START,

      END=10240,      
    };

    enum class RESOURCE_TABLE_NAME_MASK:resource_mask_t{
      START=1,

      NAME_RESOURCE=START<<0,
      USER_RESOURCE=START<<1,
      PASSWD_RESOURCE=START<<2,
      POST_RESOURCE=START<<3,
      POST_CONTENT_RESOURCE=START<<4,      

      END=START<<5,      
    };

#define HAS_RESOURCE_ID_RESERVED(resource_id) ((resource_id)>=RESOURCE_ID_RESERVED::START&&(resource_id)<=RESOURCE_ID_RESERVED::END)

#define RESOURCE_PASSWD_MAX_LEN 255
#define RESOURCE_FILE_NAME_MAX_LEN 1026
#define RESOURCE_FILE_LOCATION_MAX_LEN 10240

#define DATABASE_NAME_KEY "elevencent"

#define RESOURCE_TABLE_NAME_KEY "resource"

#define FILE_RESOURCE_TABLE_NAME_KEY "file_resource"
#define USER_RESOURCE_TABLE_NAME_KEY "user_resource"
#define NAME_RESOURCE_TABLE_NAME_KEY "name_resource"
#define PASSWD_RESOURCE_TABLE_NAME_KEY "passwd_resource"
#define POST_RESOURCE_TABLE_NAME_KEY "post_resource"
#define POST_CONTENT_RESOURCE_TABLE_NAME_KEY "post_content_resource"            

    class MariadbElevencent{
      redisContext*m_redis;
      std::unordered_map<std::string,std::string>m_properties;
      bool deleteResource(sql::Connection*conn,resource_id_t id);      
    public:
      MariadbElevencent();
      MariadbElevencent(std::unordered_map<std::string,std::string>properties);
      bool insertPasswdResource(sql::Connection*conn,resource_id_t id,resource_mask_t mask,std::string passwd);
      bool insertResource(sql::Connection*conn,resource_id_t id,resource_mask_t mask);
      bool increResourceRef(sql::Connection*conn,resource_id_t id);
      bool decreResourceRef(sql::Connection*conn,resource_id_t id);
    };    
  }
}

#endif /* _MARIADB_ELEVENCENT_H_ */
