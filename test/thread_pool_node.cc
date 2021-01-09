#include"test.h"
#include"thread_pool.h"
#include<iostream>

class MyTestCase:public TestCase{
public:
  list<ThreadPoolNode*>tasks;
  virtual void runTest(){
    for(auto task:tasks)
      task->doTask();
  }
  void pushTask(function<void*(void*)>task,void*arg,function<void(void*)>callback){
    tasks.push_back(new ThreadPoolNode(forward<function<void*(void*)>>(task),arg,forward<function<void(void*)>>(callback)));
  }
};

class Arg{
public:
  string data;
  Arg(string str):data(str){
  }
  ~Arg(){
    DEBUG_MSG("");
  }
};

class Ret{
public:
  string data;
  Ret(string str):data(str){
  }
  ~Ret(){
    DEBUG_MSG("");
  }
};

int main(){
  MyTestCase*testCase=new MyTestCase();
  int p=0;
  {
    int a=0;
    {
      int i=0;
      {
	int j=0;
	{
	  int k=0;
	  testCase->pushTask([](void*arg)->void*{
	    DEBUG_MSG("task arg: "+((Arg*)arg)->data);
	    delete (Arg*)arg;
	    return new Ret("Don't waste time on shit");	    
	  },new Arg("MacBook Pro is shit"),[](void*arg){	    
	    DEBUG_MSG("callback arg: "+((Ret*)arg)->data);
	    delete (Ret*)arg;
	  });  	  
	}
	testCase->pushTask([](void*arg)->void*{
	  DEBUG_MSG("task arg: "+((Arg*)&arg)->data);
	  return (void*)(&(Ret ret("Github is an online social website")));	  
	},(void*)(&(Arg arg("Linux is Not Unix"))),[](void*arg){	    
	  DEBUG_MSG("callback arg: "+((Ret*)arg)->data);
	});  	  
      }
    }
  }
  TestRunner runner;
  TestResult result;
  TestResultCollector collector;
  runner.addTest(testCase);
  result.addListener(&collector);
  runner.run(result);
  pause();
}


















