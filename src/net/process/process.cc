#include"process.h"
#include"connection.h"
using namespace std;
using namespace elevencent;
std::unordered_map<TcpProcessContext::STATE_IN,processHandleFunc_t> TcpProcess::Factory::ms_handleInFuncUmap;
std::unordered_map<TcpProcessContext::STATE_IN,processHandleFuncCb_t> TcpProcess::Factory::ms_handleInCbFuncUmap;
std::unordered_map<TcpProcessContext::STATE_OUT,processHandleFunc_t> TcpProcess::Factory::ms_handleOutFuncUmap;
std::unordered_map<TcpProcessContext::STATE_OUT,processHandleFuncCb_t>TcpProcess::Factory::ms_handleOutCbFuncUmap;
void*TcpProcess::handleIn(void*arg){
  auto func=Factory::getHandle(((TcpProcessContext*)((TcpConnection*)arg)->ctx)->stateIn);
  return func?func(arg):defaultHandleIn(arg);
}
void TcpProcess::handleInCb(void*arg){
  auto func=Factory::getHandleCb(((TcpProcessContext*)((TcpConnection*)arg)->ctx)->stateIn);
  if(func)func(arg);
  else defaultHandleInCb(arg);
}
void*TcpProcess::handleOut(void*arg){
  auto func=Factory::getHandle(((TcpProcessContext*)((TcpConnection*)arg)->ctx)->stateOut);
  return func?func(arg):defaultHandleOut(arg);
}
void TcpProcess::handleOutCb(void*arg){
  auto func=Factory::getHandleCb(((TcpProcessContext*)((TcpConnection*)arg)->ctx)->stateOut);
  if(func)func(arg);
  else defaultHandleOutCb(arg);
}
processHandleFunc_t TcpProcess::Factory::getHandle(const TcpProcessContext::STATE_IN state){
  return ms_handleInFuncUmap[state];    
}
processHandleFuncCb_t TcpProcess::Factory::getHandleCb(const TcpProcessContext::STATE_IN state){
  return ms_handleInCbFuncUmap[state];  
}
processHandleFunc_t TcpProcess::Factory::getHandle(const TcpProcessContext::STATE_OUT state){
  return ms_handleOutFuncUmap[state];
}
processHandleFuncCb_t TcpProcess::Factory::getHandleCb(const TcpProcessContext::STATE_OUT state){
  return ms_handleOutCbFuncUmap[state];
}
TcpProcess::Factory::Registe::Registe(const TcpProcessContext::STATE_IN state,processHandleFuncCb_t cb,processHandleFunc_t func){
  ms_handleInCbFuncUmap.insert(make_pair(state,cb));
  ms_handleInFuncUmap.insert(make_pair(state,func));        
}
TcpProcess::Factory::Registe::Registe(const TcpProcessContext::STATE_IN state,processHandleFunc_t func,processHandleFuncCb_t cb){
  ms_handleInFuncUmap.insert(make_pair(state,func));        
  ms_handleInCbFuncUmap.insert(make_pair(state,cb));
}
TcpProcess::Factory::Registe::Registe(const TcpProcessContext::STATE_OUT state,processHandleFuncCb_t cb,processHandleFunc_t func){
  ms_handleOutCbFuncUmap.insert(make_pair(state,cb));
  ms_handleOutFuncUmap.insert(make_pair(state,func));
}
TcpProcess::Factory::Registe::Registe(const TcpProcessContext::STATE_OUT state,processHandleFunc_t func,processHandleFuncCb_t cb){
  ms_handleOutFuncUmap.insert(make_pair(state,func));
  ms_handleOutCbFuncUmap.insert(make_pair(state,cb));
}
TcpProcessContext::TcpProcessContext(int events,Epoll*ep):ep(ep),stateIn(STATE_IN::START),stateOut(STATE_OUT::START),events(events){}
void TcpProcessContext::registeOnDestroyFunc(std::function<void(void*arg)>&&func){
  onDestroyFuncs.push_back(forward<std::function<void(void*arg)>>(func));
}
TcpProcessContext::~TcpProcessContext(){
  for(auto item:onDestroyFuncs)
    item(this);
}
