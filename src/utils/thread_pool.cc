#include "thread_pool.h"
using namespace elevencent;
ThreadPoolNode::ThreadPoolNode():m_pLeft(nullptr),m_pRight(nullptr),m_pParent(nullptr){

}

ThreadPool::ThreadPool(bool enableNice):m_isNiceEnabled(enableNice){
  if(!enableNice){
    
  }
}








