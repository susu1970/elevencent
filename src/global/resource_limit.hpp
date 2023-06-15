#ifndef RESOURCE_LIMIT_HPP_
#define  RESOURCE_LIMIT_HPP_

#include"global.h"
#include<string.h>
#include<unordered_map>
#include<string>

namespace elevencent{
  namespace ResourceLimit{
    class Qos{
    protected:
      volatile uint64_t last_usec=0;
    public:
      uint64_t rate;//byte/sec
      uint64_t min;
      uint64_t peekLimit(){
	if(rate==-1ul)
	  return -1ul;
	if(unlikely(!last_usec)){
	  last_usec=(volatile uint64_t)getCurUsec();    
	  return min;
	}
	volatile uint64_t sz=rate*((volatile uint64_t)getCurUsec()-last_usec)/1000000;
	return sz<min?0:sz;
      }
      void consumeLimit(uint64_t sz){
	if(sz>=min)
	  last_usec=(volatile uint64_t)getCurUsec();
      }
      Qos(uint64_t min,uint64_t rate=(uint64_t)-1):rate(rate),min(min){}
    };
    class Cpu{
    public:
      enum LIMIT_KEY:uint16_t{
	DEFAULT,
	//	IN_SIGN_UP,
	//	OUT_SIGN_UP,
	MAX
      };
      int64_t*m_limitUsecs=0;
      Cpu(int64_t*limitUsecs=0):m_limitUsecs(limitUsecs){}
      bool testAndUpdate(uint16_t key=LIMIT_KEY::DEFAULT){
	if(isTimedOut(key)){
	  updateCurUsec(key);
	  return true;
	}
	return false;
      }
      bool isTimedOut(uint16_t key=LIMIT_KEY::DEFAULT){
	if(!m_limitUsecs||m_limitUsecs[key]<0)
	  return false;
	if(!m_limitUsecs[key])
	  return true;
	if(!m_lastUsecs[key]){
	  m_lastUsecs[key]=(volatile uint64_t)getCurUsec();
	  return false;
	}
	return (volatile uint64_t)getCurUsec()-m_lastUsecs[key]>m_limitUsecs[key];
      }
      void updateCurUsec(uint16_t key=LIMIT_KEY::DEFAULT){
	m_lastUsecs[key]=(volatile uint64_t)getCurUsec();	
      }
      volatile uint64_t m_lastUsecs[LIMIT_KEY::MAX]={0};      
    };
    extern int64_t g_cpuUsecs[Cpu::LIMIT_KEY::MAX];
    class Frequency{
    public:
      enum DIMENSION:uint16_t{
	DIMENSION_DEFAULT,
	DIMENSION_SIGN_UP,
	DIMENSION_LOGIN,	
	DIMENSION_USER_SEND_MSG,
	DIMENSION_USER_REQ_MSG,
	DIMENSION_REQ_MSG_USERS,				

	DIMENSION_MAX
      };
      class Rule{
      public:
	uint64_t msec=1000;//1 second
	int limit=-1;//unlimit
      };
      class Stat{
      public:
	uint16_t maxSampleCount=0;	
	uint16_t idx=0;
	uint64_t*msecs=0;
      public:
	int prevIdx(uint16_t n){
	  if(n>maxSampleCount)
	    return -1;
	  int m=(int)idx-(int)n;
	  if(m>=0)
	    return m;
	  return m+maxSampleCount;
	}
	void sample(uint64_t msec){
	  if(!maxSampleCount)
	    return;
	  msecs[idx]=msec;
	  if(++idx>=maxSampleCount)
	    idx=0;
	}
	void setMaxSampleCount(uint16_t n){
	  maxSampleCount=n;
	  if(msecs)
	    delete[]msecs;
	  if(!n){
	    msecs=0;
	    return;
	  }
	  msecs=new uint64_t[n];
	  memset(msecs,0,sizeof(uint64_t)*n);
	}
	Stat(uint16_t maxcount=0):maxSampleCount(maxcount){
	  if(!maxcount)
	    return;
	  msecs=new uint64_t[maxcount];
	  memset(msecs,0,sizeof(uint64_t)*maxcount);
	}
	~Stat(){
	  if(msecs)
	    delete[]msecs;
	}
      };
      Rule*rules;
      Stat stats[DIMENSION_MAX];
      void setRules(Rule*rule){
	if(!(rules=rule))
	  return;
	for(int i=0;i<DIMENSION_MAX;++i)
	  stats[i].setMaxSampleCount(rules[i].limit);
      }
      bool testAndUpdate(DIMENSION d){
	testAndUpdate(getCurMsec(),d);	
      }
      bool testAndUpdate(uint64_t msec,DIMENSION d){
	Stat&st=stats[d];
	if(!st.msecs||!rules||rules[d].limit<0)
	  return false;
	if(!rules[d].limit)
	  return true;
	int idx=st.prevIdx(rules[d].limit);
	if(idx<0||msec-st.msecs[idx]<rules[d].msec)
	  return true;
	st.sample(msec);
	return false;
      }
      Frequency(Rule*rule=0){
	setRules(rule);	
      }
    };
    extern Frequency::Rule g_rules[Frequency::DIMENSION_MAX];
    class Size{
      std::unordered_map<std::string,uint64_t>limit;
    public:
      uint64_t operator[](std::string key){
	try{
	  return limit.at(key);
	}catch(const std::out_of_range&){
	  return -1ul;	  
	}
      }
      void insert(std::string key,uint64_t value){
	limit[key]=value;
      }
    };
    extern Size g_sizeLimit;
  }
}


#endif
