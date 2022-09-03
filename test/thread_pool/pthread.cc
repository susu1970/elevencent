#include"test.h"

void*func(void*arg){
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,0);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,0);
  while(1){
    DEBUG_PRETTY_MSG("");
    DEBUG_PRETTY_MSG("");
    DEBUG_PRETTY_MSG("");
    DEBUG_PRETTY_MSG("");
    DEBUG_PRETTY_MSG("");
    DEBUG_PRETTY_MSG("");
    DEBUG_PRETTY_MSG("");
    DEBUG_PRETTY_MSG("");
    DEBUG_PRETTY_MSG("");
  }
  pthread_testcancel();
  DEBUG_PRETTY_MSG("");  
}

int main(){
  pthread_t tid=0;
  pthread_create(&tid,0,func,0);
  sleep(1);
  pthread_cancel(tid);
  cout<<"main exit"<<endl;
  pthread_exit(0);
}
