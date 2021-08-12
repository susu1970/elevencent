#include"test.h"
#include"mariadb_pool.h"
#include"thread_pool.h"
using namespace sql;
pthread_mutex_t conn_lock;
void deleteTask(Connection*conn,int id){
  try{
    unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("delete from tasks where id = ?"));
    stmnt->setInt(1,id);
    stmnt->executeQuery();
  }catch(sql::SQLException&e){
    cerr<<"Error deleting tasks: "<<e.what()<<endl;
  }
}

void updateTaskStatus(Connection*conn,int id,bool completed){
  try{
    unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("update tasks set completed = ? where id = ?"));
    stmnt->setBoolean(1,completed);
    stmnt->setInt(2,id);
    stmnt->executeQuery();
  }catch(sql::SQLException&e){
    cerr<<"Error updating task status: "<<e.what()<<endl;
  }
}

void addTask(Connection*conn,string description){
  try{
    unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("insert into tasks (description) values (?)"));
    stmnt->setString(1,description);
    stmnt->executeQuery();
  }catch(sql::SQLException&e){
    cerr<<"Error inserting new task: "<<e.what()<<endl;
  }
}

void showTasks(Connection*conn,string type){
  //  pthread_mutex_lock(&conn_lock);
  if(!conn)
    type="null "+type;
  //    cout<<"------------"<<type<<">-----------"<<endl;
  if(conn)
    try{
      unique_ptr<sql::Statement> stmnt(conn->createStatement());
      sql::ResultSet*res=stmnt->executeQuery("select * from tasks");
      while(res->next()){
	//	cout<<"id: "<<res->getInt(1)<<", description: "<<res->getString(2)<<", completed: "<<res->getBoolean(3)<<endl;
      }
    }catch(sql::SQLException&e){
      cerr<<"error selecting tasks: "<<e.what()<<endl;
    }
  //  pthread_mutex_unlock(&conn_lock);  
  //  cout<<"------------</"<<type<<">-----------"<<endl;  
}
int main(int argc,char*argv[]){
  srand(time(0));
  ThreadPool thrPool;
  unordered_map<string,string> properties;
  long long loopTimes=0;
  properties["userName"]="app_user";
  properties["password"]="Password123!";
  properties["OPT_RECONNECT"]="true";
  properties["hostName"]="localhost";
  properties["port"]="3306";
  properties["schema"]="todo";
  pthread_mutex_t pool_lock;
  pthread_mutex_init(&pool_lock,0);
  pthread_mutex_t umap_lock;
  pthread_mutex_init(&umap_lock,0);
  pthread_mutex_init(&conn_lock,0);
  while(1){
    try{
      properties[MARIADB_POOL_CACHE_NUM_KEY]=to_string(rand()%16);
      properties[MARIADB_POOL_MAX_NUM_KEY]=to_string(rand()%16);
      MariadbPool pool(properties);
      unordered_map<Connection*,bool>p_conns;
      for(int i=0;i<30000;++i)
	thrPool.pushTask([&](void*arg)->void*{
	  pthread_mutex_lock(&umap_lock);
	  pthread_mutex_lock(&pool_lock);
	  p_conns[MariadbPool::getConn(&pool)];
	  pthread_mutex_unlock(&umap_lock);	
	  pthread_mutex_unlock(&pool_lock);	
	  return 0;
	},0,[&](void*arg){
	  pthread_mutex_lock(&umap_lock);
	  if(p_conns.empty()){
	    pthread_mutex_unlock(&umap_lock);
	    return;
	  }
	  Connection*p_conn=p_conns.begin()->first;
	  pthread_mutex_lock(&pool_lock);
	  MariadbPool::recycleConn(&pool,p_conn);
	  p_conns.erase(p_conn);
	  pthread_mutex_unlock(&umap_lock);	
	  pthread_mutex_unlock(&pool_lock);
	});
      bool finished=false;
      thrPool.clearAllThrs([&](void*arg){
	ThreadPool*p=(ThreadPool*)arg;
	p->test1();
	pthread_mutex_lock(&umap_lock);      
	for(auto iter=p_conns.begin();iter!=p_conns.end();++iter){
	  if(iter->first)
	    iter->first->close();
	  delete iter->first;
	}
	pthread_mutex_unlock(&umap_lock);      
	DEBUG_MSG("clearAllThrs finished\nloopTimes: "<<loopTimes);                  
	finished=true;
      },&thrPool);
      while(!finished);
      finished=false;
      ++loopTimes;
    }catch(SQLException&e){
      DEBUG_MSG("exception: "<<e.what());
    }
    //    pause();
  }
  return 0;
}
