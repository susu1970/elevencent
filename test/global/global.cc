#include"test.h"


int main(int argc,char**argv){
  cout<<"CUR_TIME_CSTR: "<<CUR_TIME_CSTR<<endl;
  cout<<"LOCATE_INFO_STR: "<<LOCATE_INFO_STR<<endl;
  cout<<"LOCATE_PRETTY_INFO_STR: "<<LOCATE_PRETTY_INFO_STR<<endl;
  DEBUG_MSG("debug msg");
  DEBUG_PRETTY_MSG("debug pretty msg");
  DEBUG_ASSERT(1,"debug assert 1");
  //  DEBUG_ASSERT(0,"debug assert 0");
  DEBUG_PRETTY_ASSERT(1,"debug pretty assert 1");
  //  DEBUG_PRETTY_ASSERT(0,"debug pretty assert 0");
  ASSERT(1,"assert 1");
  //  ASSERT(0,"assert 0");
  DEBUG_MSG("g_processNum: "<<g_processNum);
  return 0;
}
