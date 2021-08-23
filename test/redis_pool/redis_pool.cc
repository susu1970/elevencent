#include"test.h"

#include"test.h"
#include"mariadb_pool.h"
#include"thread_pool.h"
#include"redis_pool.h"

pthread_mutex_t conn_lock;

unordered_map<int,string>redisReplyUMap{
  {REDIS_REPLY_STRING,"REDIS_REPLY_STRING"},
  {REDIS_REPLY_ARRAY,"REDIS_REPLY_ARRAY"},
  {REDIS_REPLY_INTEGER,"REDIS_REPLY_INTEGER"},
  {REDIS_REPLY_NIL,"REDIS_REPLY_NIL"},
  {REDIS_REPLY_STATUS,"REDIS_REPLY_STATUS"},
  {REDIS_REPLY_ERROR,"REDIS_REPLY_ERROR"}
};

void printReply(redisReply*reply){
  return;
  if(!reply){
    DEBUG_MSG("!reply");
    return;
  }
  DEBUG_MSG("reply: "<<reply<<"\ntype: "<<redisReplyUMap[reply->type]<<"\ninteger: "<<reply->integer<<"\nstr: "<<reply->str<<"\nelements: "<<reply->elements);
  for(int i=0;i<reply->elements;++i)
    printReply(reply->element[i]);
  
}

int main(int argc,char*argv[]){
  srand(time(0));
  ThreadPool thrPool;
  unordered_map<string,string> properties;
  long long loopTimes=0;
  properties[REDIS_POOL_HOST_NAME_KEY]="127.0.0.1";
  properties[REDIS_POOL_PORT_KEY]="6379";
  pthread_mutex_t pool_lock;
  pthread_mutex_init(&pool_lock,0);
  pthread_mutex_t umap_lock;
  pthread_mutex_init(&umap_lock,0);
  pthread_mutex_init(&conn_lock,0);
  while(1){
    properties[REDIS_POOL_CACHE_NUM_KEY]=to_string(rand()%16);
    properties[REDIS_POOL_MAX_NUM_KEY]=to_string(rand()%16);
    RedisPool pool(properties);
    unordered_map<redisContext*,bool>p_conns;
    for(int i=0;i<30000;++i)
      thrPool.pushTask([&](void*arg)->void*{	
	pthread_mutex_lock(&umap_lock);
	pthread_mutex_lock(&pool_lock);
	redisContext*context=RedisPool::getRedisContext(&pool);
	if(!context){
	  DEBUG_PRETTY_MSG("!context");
	  pthread_mutex_unlock(&umap_lock);	
	  pthread_mutex_unlock(&pool_lock);
	  return 0;
	}
	p_conns[context]=true;
	redisReply*reply;
	reply=(redisReply*)redisCommand(context,"SET susu8848 1970");
	printReply(reply);
	freeReplyObject(reply);	
	printReply(reply=(redisReply*)redisCommand(context,"GET susu8848"));
	freeReplyObject(reply);	
	p_conns.erase(context);
	RedisPool::recycleRedisContext(&pool,context);
	pthread_mutex_unlock(&umap_lock);	
	pthread_mutex_unlock(&pool_lock);
	return 0;
      },0,[](void*arg){});
    bool finished=false;
    thrPool.clearAllThrs([&](void*arg){
      ThreadPool*p=(ThreadPool*)arg;
      p->test1();
      pthread_mutex_lock(&umap_lock);    
      for(auto iter=p_conns.begin();iter!=p_conns.end();++iter)
	RedisPool::recycleRedisContext(0,(redisContext*)iter->first);
      pthread_mutex_unlock(&umap_lock);      
      DEBUG_MSG("clearAllThrs finished\nloopTimes: "<<loopTimes);                  
      finished=true;
    },&thrPool);
    while(!finished);
    finished=false;
    ++loopTimes;
  }
  return 0;
}
