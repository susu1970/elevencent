#include"net_process.h"

using namespace qt_elevencent;
netProcessHandleFunc_t g_handleInFuncs[NET_PROCESS_MAX_STATE]={0};
NetProcess::Registe::Registe(NetModel::STATE_IN state,netProcessHandleFunc_t handle){
  g_handleInFuncs[(int)state]=handle;
}
void NetProcess::handleIn(NetThread*thr){
  if((int)thr->m_stateIn<0||(int)thr->m_stateIn>=NET_PROCESS_MAX_STATE||!g_handleInFuncs[(int)thr->m_stateIn]){
    thr->m_retIn|=NetModel::UNKNOWN_STATE;
    return;
  }
  g_handleInFuncs[(int)thr->m_stateIn](thr);
}
