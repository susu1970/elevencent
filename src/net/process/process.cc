#include<string.h>
#include"process.h"
#include"connection.h"
using namespace std;
using namespace elevencent;
processHandleFunc_t g_handleInFuncs[TCP_PROCESS_CONTEXT_MAX_STATE]={0};
processHandleFuncCb_t g_handleInCbFuncs[TCP_PROCESS_CONTEXT_MAX_STATE]={0};
processHandleFunc_t g_handleOutFuncs[TCP_PROCESS_CONTEXT_MAX_STATE]={0};
processHandleFuncCb_t g_handleOutCbFuncs[TCP_PROCESS_CONTEXT_MAX_STATE]={0};      

void*TcpProcess::handleIn(void*arg){
  auto func=g_handleInFuncs[(short)(((TcpProcessContext*)((TcpConnection*)arg)->ctx)->stateIn)];
  return func?func(arg):defaultHandleIn(arg);
}
void TcpProcess::handleInCb(void*arg){
  auto func=g_handleInCbFuncs[(short)(((TcpProcessContext*)((TcpConnection*)arg)->ctx)->stateInCb)];
  if(func)func(arg);
  else defaultHandleInCb(arg);
}
void*TcpProcess::handleOut(void*arg){
  auto func=g_handleOutFuncs[(short)(((TcpProcessContext*)((TcpConnection*)arg)->ctx)->stateOut)];
  return func?func(arg):defaultHandleOut(arg);
}
void TcpProcess::handleOutCb(void*arg){
  auto func=g_handleOutCbFuncs[(short)(((TcpProcessContext*)((TcpConnection*)arg)->ctx)->stateOutCb)];
  if(func)func(arg);
  else defaultHandleOutCb(arg);
}
TcpProcess::Factory::Registe::Registe(const TcpProcessContext::STATE_IN state,processHandleFuncCb_t cb,processHandleFunc_t func){
  g_handleInFuncs[(short)state]=func;
  g_handleInCbFuncs[(short)state]=cb;    
}
TcpProcess::Factory::Registe::Registe(const TcpProcessContext::STATE_IN state,processHandleFunc_t func,processHandleFuncCb_t cb){
  g_handleInFuncs[(short)state]=func;
  g_handleInCbFuncs[(short)state]=cb;    
}
TcpProcess::Factory::Registe::Registe(const TcpProcessContext::STATE_OUT state,processHandleFuncCb_t cb,processHandleFunc_t func){
  g_handleOutFuncs[(short)state]=func;
  g_handleOutCbFuncs[(short)state]=cb;    
}
TcpProcess::Factory::Registe::Registe(const TcpProcessContext::STATE_OUT state,processHandleFunc_t func,processHandleFuncCb_t cb){
  g_handleOutFuncs[(short)state]=func;
  g_handleOutCbFuncs[(short)state]=cb;  
}
void TcpProcessContext::registeOnDestroyFunc(std::function<void(void*arg)>&&func){
  onDestroyFuncsLock.lock();
  onDestroyFuncs.push_back(forward<std::function<void(void*arg)>>(func));
  onDestroyFuncsLock.unlock();  
}
void TcpProcessContext::registeOnDestroyFunc(STATE_IN state,std::function<void(void*arg)>&&func,bool doOld){
  if(onDestroyFuncsIn.contains(state)&&doOld)
    onDestroyFuncsIn[state](this);
  onDestroyFuncsIn[state]=forward<std::function<void(void*arg)>>(func);  
}
void TcpProcessContext::registeOnDestroyFunc(STATE_OUT state,std::function<void(void*arg)>&&func,bool doOld){
  if(onDestroyFuncsOut.contains(state)&&doOld)
    onDestroyFuncsOut[state](this);
  onDestroyFuncsOut[state]=forward<std::function<void(void*arg)>>(func);  
}
TcpProcessContext::~TcpProcessContext(){
  for(auto&item:onDestroyFuncs)
    item(this);
  for(auto&item:onDestroyFuncsIn)
    item.second(this);
  for(auto&item:onDestroyFuncsOut)
    item.second(this);
}
size_t TcpProcessContext::List::size(){
  return fuzzySize;
}
bool TcpProcessContext::List::rm(TcpProcessContext::List::Node*node){
  volatile Node*prev=head;
  volatile Node*cur=head->next;
  while(cur&&cur!=node){
    prev=cur;
    cur=cur->next;
  }
  if(!cur)
    return false;
  while(!CAS_BOOL((uint64_t*)&tail->next,0,1))
    sched_yield();
  --fuzzySize;
  if(tail!=cur){
    tail->next=0;
    prev->next=cur->next;
  }else{
    prev->next=0;
    tail=prev;
  }
  return true;  
}
TcpProcessContext::List::List(){
  head=tail=new Node();
}
TcpProcessContext::List::Node*TcpProcessContext::List::front(){
  return head->next;
}
bool TcpProcessContext::List::push_back(Node*p){
  p->next=0;
  while(!CAS_BOOL((uint64_t*)&tail->next,0,(uint64_t)p))
    sched_yield();
  if(fuzzySize>=fuzzyMaxSize){
    tail->next=0;
    return false;
  }
  ++fuzzySize;
  tail=p;
  return true;
}
TcpProcessContext::List::~List(){
  volatile Node*q;
  for(volatile Node*p=head;p;p=q){
    q=p->next;
    delete p;    
  }
}
