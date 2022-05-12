#include"process.h"
#include"connection.h"

using namespace std;
using namespace elevencent;
std::unordered_map<TcpProcessContext::STATE,processHandleFunc_t> TcpProcess::Factory::ms_handleInFuncUmap;
std::unordered_map<TcpProcessContext::STATE,processHandleFuncCb_t> TcpProcess::Factory::ms_handleInCbFuncUmap;
std::unordered_map<TcpProcessContext::STATE,processHandleFunc_t> TcpProcess::Factory::ms_handleOutFuncUmap;
std::unordered_map<TcpProcessContext::STATE,processHandleFuncCb_t>TcpProcess::Factory::ms_handleOutCbFuncUmap;      

void*TcpProcess::handleIn(void*arg){
  auto func=Factory::getHandleIn(((TcpProcessContext*)((TcpConnection*)arg)->ctx)->state);
  return func?func(arg):arg;
}
void TcpProcess::handleInCb(void*arg){
  auto func=Factory::getHandleInCb(((TcpProcessContext*)((TcpConnection*)arg)->ctx)->state);
  if(!func){
    DEBUG_MSG("unregisted process state("<<((int)((TcpProcessContext*)((TcpConnection*)arg)->ctx)->state)<<"), close fd("<<((TcpConnection*)arg)->fd<<")");
    delete ((TcpConnection*)arg);
    return;
  }
  func(arg);
}
void*TcpProcess::handleOut(void*arg){
  auto func=Factory::getHandleOut(((TcpProcessContext*)((TcpConnection*)arg)->ctx)->state);
  return func?func(arg):arg;  
}
void TcpProcess::handleOutCb(void*arg){
  auto func=Factory::getHandleOutCb(((TcpProcessContext*)((TcpConnection*)arg)->ctx)->state);
  if(!func)return;
  func(arg);  
}
TcpProcessContext::TcpProcessContext(Epoll*ep_):ep(ep_),state(STATE::HEAD),off(0){}
TcpProcessContext::~TcpProcessContext(){}
processHandleFunc_t TcpProcess::Factory::getHandleIn(const TcpProcessContext::STATE state){
  return ms_handleInFuncUmap.find(state)==ms_handleInFuncUmap.end()?0:ms_handleInFuncUmap[state];
}
processHandleFuncCb_t TcpProcess::Factory::getHandleInCb(const TcpProcessContext::STATE state){
  return ms_handleInCbFuncUmap.find(state)==ms_handleInCbFuncUmap.end()?0:ms_handleInCbFuncUmap[state];
}
processHandleFunc_t TcpProcess::Factory::getHandleOut(const TcpProcessContext::STATE state){
  return ms_handleOutFuncUmap.find(state)==ms_handleOutFuncUmap.end()?0:ms_handleOutFuncUmap[state];
}
processHandleFuncCb_t TcpProcess::Factory::getHandleOutCb(const TcpProcessContext::STATE state){
  return ms_handleOutCbFuncUmap.find(state)==ms_handleOutCbFuncUmap.end()?0:ms_handleOutCbFuncUmap[state];
}
void TcpProcess::Factory::registeHandleIn(const TcpProcessContext::STATE state,processHandleFunc_t func){
  ms_handleInFuncUmap.insert(make_pair(state,func));
}
void TcpProcess::Factory::registeHandleInCb(const TcpProcessContext::STATE state,processHandleFuncCb_t func){
  ms_handleInCbFuncUmap.insert(make_pair(state,func));  
}
void TcpProcess::Factory::registeHandleOut(const TcpProcessContext::STATE state,processHandleFunc_t func){
  ms_handleOutFuncUmap.insert(make_pair(state,func));  
}
void TcpProcess::Factory::registeHandleOutCb(const TcpProcessContext::STATE state,processHandleFuncCb_t func){
  ms_handleOutCbFuncUmap.insert(make_pair(state,func)); 
}
void TcpProcess::Factory::registe(const TcpProcessContext::STATE state,processHandleFunc_t in,processHandleFuncCb_t inCb,processHandleFunc_t out,processHandleFuncCb_t outCb){
  registeHandleIn(state,in);
  registeHandleInCb(state,inCb);
  registeHandleOut(state,out);
  registeHandleOutCb(state,outCb);  
}
TcpProcess::Factory::Registe::Registe(const TcpProcessContext::STATE state,processHandleFunc_t in,processHandleFuncCb_t inCb,processHandleFunc_t out,processHandleFuncCb_t outCb){
  TcpProcess::Factory::registe(state,in,inCb,out,outCb);
}
