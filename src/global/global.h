#ifndef ELEVENCENT_GLOBAL_H_
#define ELEVENCENT_GLOBAL_H_

#include<cassert>
#include<iostream>
#include<string>
#include<stdio.h>
#include<syslog.h>


namespace elevencent{
  
#define BETA 1
#define DEBUG 1
  
#define CUR_TIME_CSTR ({time_t now=time(0);char buf[80];		\
      strftime(buf,sizeof(buf),"%b %e %Y.%X",localtime(&now));buf;})

#define LOCATE_INFO_STR (std::string("compile time: ")+std::string(__DATE__)+"."+std::string(__TIME__)+"\nrunning time: "+std::string(CUR_TIME_CSTR)+"\nrunning timestamp: "+std::to_string(time(0))+"\nfile: "+std::string(__FILE__)+"\nline: "+std::to_string(__LINE__)+"\nfunc: "+__PRETTY_FUNCTION__)
#define LOCATE_PRETTY_INFO_STR (std::string("running timestamp: ")+std::to_string(time(0))+",file: "+std::string(__FILE__)+",line: "+std::to_string(__LINE__))
#define DEBUG_MSG(msg) ((DEBUG)&&(std::cout<<"\n------<DEBUG MSG>-----\n"<<LOCATE_INFO_STR<<"\n------<msg>------\n"<<std::string("")+msg<<"\n------</msg>------\n------</DEBUG MSG>-----\n"))
#define DEBUG_PRETTY_MSG(msg) ((DEBUG)&&(std::cout<<LOCATE_PRETTY_INFO_STR<<", msg: "<<msg<<"\n"))
#define DEBUG_ASSERT(cond,msg) ({(DEBUG)&&(!(cond))&&(std::cout<<"\n------<DEBUG ASSERT>-----\n"<<LOCATE_INFO_STR<<"\n------</DEBUG ASSERT>-----\n");assert(cond);})
#define DEBUG_PRETTY_ASSERT(cond,msg) ({(DEBUG)&&(!(cond))&&(std::cout<<"assert failed("<<(cond)<<"), "<<LOCATE_PRETTY_INFO_STR<<", msg: "<<std::string("")+msg<<"\n");assert(cond);})
#define SYSLOG(priority,format,...) (syslog(priority,std::string(LOCATE_INFO_STR+"\n"+format).c_str(),##__VA_ARGS__))
#define ASSERT(cond,msg) ({if(!cond){SYSLOG(LOG_ERR,"assert failed: %s",msg);assert(cond);}})

}

#endif






