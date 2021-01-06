#include<pthread.h>
#include<iostream>

using namespace std;

int main(){
  pthread_mutex_t pt;
  pthread_mutex_init(&pt,0);
  pthread_mutex_lock(&pt);
  cout<<"lock"<<endl;
  pthread_mutex_unlock(&pt);
  cout<<"unlock"<<endl;
}













