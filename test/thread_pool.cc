#include"test.h"
//#include"test.h"
#include<vector>
using namespace std;

#include"thread_pool.h"
using namespace elevencent;

class PushTaskTestCase{//}:public TestCase{
  ThreadPool*m_pool;
public:
  PushTaskTestCase(vector<short>nicevec){
    m_pool=new ThreadPool(true);
    for(auto nice:nicevec){
      m_pool->pushTask([&nice](void*arg)->void*{
	return nullptr;},nullptr,[&nice](void*ret){
	},nice);
    }
    for(short i=0;i<nicevec.size();++i){
      m_pool->consumeTask(i);
      m_pool->pushTask([i](void*arg)->void*{
	return nullptr;},nullptr,[i](void*ret){
	},i);
    }
  }
  virtual void runTest(){
  }
};

vector<vector<short>>vecs;

int main(){
  cout<<endl<<endl;
  vecs.push_back({
      10,20,30,40,50,60,70,-10
    });
  for(short i=0;i<4;++i){
    if(i==0){
      vecs.push_back({
	  0
	});
    }else if(i==1){
      vecs.push_back({
	  0,-10
	});
      vecs.push_back({
	  0,0
	});
      vecs.push_back({
	  0,10
	});
    }else if(i==2){
      vecs.push_back({
	  0,10,-10
	});
      vecs.push_back({
	  0,10,0
	});
      vecs.push_back({
	  0,10,5
	});
      vecs.push_back({
	  0,10,10
	});
      vecs.push_back({
	  0,10,20
	});
      vecs.push_back({
	  0,-10,-20
	});
      vecs.push_back({
	  0,-10,-10
	});
      vecs.push_back({
	  0,-10,-5
	});
      vecs.push_back({
	  0,-10,0
	});
      vecs.push_back({
	  0,-10,10
	});
      vecs.push_back({
	  0,0,-10
	});
      vecs.push_back({
	  0,0,0
	});
      vecs.push_back({
	  0,0,10
	});

    }else if(i==3){
      vecs.push_back({     
	  0,10,-10,-20
	});
      vecs.push_back({
	  0,10,-10,-10
	});
      vecs.push_back({
	  0,10,-10,-5
	});
      vecs.push_back({
	  0,10,-10,0
	});
      vecs.push_back({
	  0,10,-10,5
	});
      vecs.push_back({
	  0,10,-10,10
	});
      vecs.push_back({
	  0,10,-10,20
	});

      vecs.push_back({
	  0,10,5,-10
	});
      vecs.push_back({
	  0,10,5,0
	});
      vecs.push_back({
	  0,10,5,3
	});
      vecs.push_back({
	  0,10,5,5
	});
      vecs.push_back({
	  0,10,5,7
	});
      vecs.push_back({
	  0,10,5,10
	});
      vecs.push_back({
	  0,10,5,15
	});

      vecs.push_back({
	  0,10,10,-10
	});
      vecs.push_back({
	  0,10,10,0
	});
      vecs.push_back({
	  0,10,10,5
	});
      vecs.push_back({
	  0,10,10,10
	});
      vecs.push_back({
	  0,10,10,20
	});

      vecs.push_back({
	  0,10,5,-10
	});
      vecs.push_back({
	  0,10,5,0
	});
      vecs.push_back({
	  0,10,5,3
	});
      vecs.push_back({
	  0,10,5,5
	});
      vecs.push_back({
	  0,10,5,7
	});
      vecs.push_back({
	  0,10,5,10
	});
      vecs.push_back({
	  0,10,5,20
	});


      vecs.push_back({
	  0,10,20,-10
	});
      vecs.push_back({
	  0,10,20,0
	});
      vecs.push_back({
	  0,10,20,5
	});
      vecs.push_back({
	  0,10,20,10
	});
      vecs.push_back({
	  0,10,20,15
	});
      vecs.push_back({
	  0,10,20,20
	});
      vecs.push_back({
	  0,10,20,25
	});

      vecs.push_back({
	  0,-10,-20,-30
	});
      vecs.push_back({
	  0,-10,-20,-20
	});
      vecs.push_back({
	  0,-10,-20,-15
	});
      vecs.push_back({
	  0,-10,-20,-10
	});
      vecs.push_back({
	  0,-10,-20,-5
	});
      vecs.push_back({
	  0,-10,-20,0
	});
      vecs.push_back({
	  0,-10,-20,10
	});
      
      vecs.push_back({
	  0,-10,-10,-20
	});
      vecs.push_back({
	  0,-10,-10,-10
	});
      vecs.push_back({
	  0,-10,-10,-5
	});
      vecs.push_back({
	  0,-10,-10,0
	});
      vecs.push_back({
	  0,-10,-10,10
	});

      vecs.push_back({
	  0,-10,-5,-20
	});
      vecs.push_back({
	  0,-10,-5,-10
	});
      vecs.push_back({
	  0,-10,-5,-7
	});
      vecs.push_back({
	  0,-10,-5,-5
	});
      vecs.push_back({
	  0,-10,-5,-3
	});
      vecs.push_back({
	  0,-10,-5,0
	});
      vecs.push_back({
	  0,-10,-5,10
	})
;
      vecs.push_back({
	  0,-10,0,-20
	});
      vecs.push_back({
	  0,-10,0,-10
	});
      vecs.push_back({
	  0,-10,0,-5
	});
      vecs.push_back({
	  0,-10,0,0
	});
      vecs.push_back({
	  0,-10,0,10
	});

      vecs.push_back({
	  0,-10,10,-20
	});
      vecs.push_back({
	  0,-10,10,-10
	});
      vecs.push_back({
	  0,-10,10,-5
	});
      vecs.push_back({
	  0,-10,10,0
	});
      vecs.push_back({
	  0,-10,10,5
	});
      vecs.push_back({
	  0,-10,10,10
	});
      vecs.push_back({
	  0,-10,10,20
	});

      vecs.push_back({
	  0,0,-10,-20
	});
      vecs.push_back({
	  0,0,-10,-5
	});
      vecs.push_back({
	  0,0,-10,-10
	});
      vecs.push_back({
	  0,0,-10,0
	});
      vecs.push_back({
	  0,0,-10,10
	});

      vecs.push_back({
	  0,0,0,-10
	});
      vecs.push_back({
	  0,0,0,0
	});
      vecs.push_back({
	  0,0,0,10
	});

      vecs.push_back({
	  0,0,10,-10
	});
      vecs.push_back({
	  0,0,10,0
	});
      vecs.push_back({
	  0,0,10,5
	});
      vecs.push_back({
	  0,0,10,10
	});
      vecs.push_back({
	  0,0,10,20
	});
    }
  }
  vecs.push_back({
      10,20,30,40,50,60,70,-10
    }); 
  vecs.push_back({
      10,20,30,40,50,60,70,80,-10
    });
  vecs.push_back({
      10,20,30,40,50,60,70,80,90,-10
    });
  vecs.push_back({
      10,20,30,40,50,60,70,80,90,100,-10
    });
  vecs.push_back({
      10,20,30,40,50,60,70,80,90,100,110,-10
    });
  vecs.push_back({
      10,20,30,40,50,60,100,110,70,80,90,120,-10,140,15,-1115,1116,1115,1117,1118,1119,35,25,45,55,115,65
    });
  srand(time(0));
  //  goto test;
  for(short i=0;i<30000;++i){
    short k=rand()%1500+3000;
    vector<short>vec;
    for(short j=0;j<k;++j){
      vec.push_back(rand()%12008);
    }
    vecs.push_back(vec);
  }
 test:
  vecs.push_back({
      10,20,30,40,50,60,100,110,70,80,90,120,-10,140,15,-1115,1116,1115,1117,1118,1119,35,25,45,55,115,65
    });
  DEBUG_MSG("vecs.size(): "<<vecs.size());
    
  for(auto vec:vecs){
    (new PushTaskTestCase(vec));
  }
}


















