#ifndef _NET_PROCESS_H_
#define _NET_PROCESS_H_
#include"net_thread.h"
namespace qt_elevencent{
  typedef void(*netProcessHandleFunc_t)(void*);  
  class NetProcess{
  public:
    class Registe{
    public:
      Registe(NetModel::STATE_IN,netProcessHandleFunc_t handle);
    };
    static void handleIn(NetThread*);
  };
#define NET_PROCESS_REGISTE_IN(name,state,...) static NetProcess::Registe s_netProcessRegisteIn##name(state,##__VA_ARGS__)
}

#endif
