#include<cppunit/TestCase.h>
#include<cppunit/TestRunner.h>
#include<cppunit/TestResult.h>
#include<cppunit/TestResultCollector.h>
#include<cppunit/TextOutputter.h>

#include"global.h"

using namespace std;
using namespace CppUnit;

class MyTestCase:public TestCase{
public:
  virtual void runTest(){
    DEBUG_MSG("");
  }
};

int main(){
  MyTestCase*tc=new MyTestCase();
  TestRunner r;
  r.addTest(tc);
  TestResultCollector rc;
  TestResult result;
  result.addListener(&rc);
  r.run(result);
  TextOutputter op(&rc,cout);
  op.write();
}


















