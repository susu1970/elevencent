#include"test.h"
#include"thread_pool.h"
#include<iostream>

void*task(void*){
  DEBUG_MSG("task");
}

void callback(void*){
  DEBUG_MSG("callback");
}

int main(){
  ThreadPoolNode*node=new ThreadPoolNode();
  node->doTask();
}


















