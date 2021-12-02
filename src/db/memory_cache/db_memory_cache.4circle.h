#ifndef _DB_MEMORY_CACHE_4CIRCLE_H_
#define _DB_MEMORY_CACHE_4CIRCLE_H_
namespace elevencent{
  enum class DB_MEMORY_CACHE_TYPE:char{
    THROUGH,
    BACK,
    AROUND,
    DFT=THROUGH
  };
  enum class DB_MEMORY_CACHE_REPLACEMENT:char{
    LRU,
    RR,
    FIFO,
    DFT=RR
  };
  class DbMemoryCache;
}

#endif /* _DB_MEMORY_CACHE_H4CIRCLE_ */
