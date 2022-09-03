#ifndef _ELEVENCENT_PROCESS_INTERFACE_H_
#define _ELEVENCENT_PROCESS_INTERFACE_H_

namespace elevencent{
  class ProcessInterface{
  public:
    ProcessInterface(){}
    virtual void*handleIn(void*arg)=0;
    virtual void handleInCb(void*arg)=0;
    virtual void*handleOut(void*arg)=0;
    virtual void handleOutCb(void*arg)=0;
    virtual ~ProcessInterface(){}
  };
}

#endif
