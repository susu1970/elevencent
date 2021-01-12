
#include"test.h"

#include"thread_pool.h"

class PushTaskTestCase:public TestCase{
  ThreadPool*m_pool;
public:
  PushTaskTestCase(vector<short>nicevec){
    m_pool=new ThreadPool(true);
    cout<<"vecs: ";
    for(auto nice:nicevec)
      cout<<nice<<", ";
    cout<<endl;
    for(auto nice:nicevec){
      m_pool->pushTask([](void*arg)->void*{return nullptr;},nullptr,[](void*ret){},nice);
    }
    cout<<endl;
    m_pool->traverseLayer();
    cout<<"\n\n"<<endl;
  }
  virtual void runTest(){
  }
};

vector<vector<short>>vecs;

int main(){
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
 l:
  vecs.push_back({
      10,20,30,40,50,60,100,110,70,80,90,120,-10,140,15,-1115,1116,1115,1117,1118,1119,35,25,45,55,115,65
    });

  srand(time(0));
  for(short i=111110;i<1000;++i){
    vector<short>vec;
    short k=rand()%1000;
    for(short j=0;j<k;++j){
      vec.push_back(rand());
    }
    vecs.push_back(vec);
  }
  TestRunner runner;
  for(auto vec:vecs){
    runner.addTest(new PushTaskTestCase(vec));
  }
  TestResult result;
  TestResultCollector collector;
  result.addListener(&collector);  
  runner.run(result);
}


















