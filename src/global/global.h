#ifndef ELEVENCENT_GLOBAL_H_
#define ELEVENCENT_GLOBAL_H_

#include<iostream>
#include<string>

namespace elevencent{
  
#define BETA 1
#define DEBUG 1
  
#define CUR_TIME_CSTR ({time_t now=time(0);char buf[80];		\
      strftime(buf,sizeof(buf),"%b %e %Y.%X",localtime(&now));buf;})
#define DEBUG_MSG(msg) (DEBUG)&&(std::cout<<"\n------<DEBUG MSG>-----\ncompile time: "<<__DATE__<<"."<<__TIME__<<"\nrunning time: "<<CUR_TIME_CSTR<<"\nrunning timestamp: "<<time(0)<<"\nfile: "<<__FILE__<<"\nline: "<<__LINE__<<"\nfunc: "<<__PRETTY_FUNCTION__<<"\n------<msg>------\n"<<std::string("")+msg<<"\n------</msg>------\n------</DEBUG MSG>-----\n")
#define DEBUG_PRETTY_MSG(msg) (DEBUG)&&(std::cout<<__FUNCTION__<<", msg: "<<std::string("")+msg<<"\n");

}

#endif






