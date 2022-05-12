#include"rsa.h"
#include"global.h"

#include<time.h>
#include<stdlib.h>
#include<iostream>
using namespace std;
using namespace elevencent;

#define RSA_BIT_SIZE 2048
RSA::Key::Key(){}
RSA::Key::Key(const RSA::Key&a){
  if(this!=&a){
    n=a.n;
    x=a.x;
    str=a.str;
  }
}
RSA::Key&RSA::Key::operator=(const RSA::Key&a){
  if(this!=&a){
    n=a.n;
    x=a.x;
    str=a.str;
  }
  return *this;
}
RSA::Key&RSA::Key::operator=(const std::string&str){
  return *this=RSA::str2key(str);
}  
RSA::Key::Key(const string&str){
  *this=RSA::str2key(str);
}
string RSA::key2str(const Key&key){
  string str=key.n.get_str(32);
  string len=to_string(str.size());
  while(len.size()<5)
    len="0"+len;
  string ret=len+str;
  str=key.x.get_str(32);
  len=to_string(str.size());
  while(len.size()<5)
    len="0"+len;
  ret=ret+len+str;
  return ret;
}
RSA::Key RSA::str2key(const std::string&str){
  Key key;
  if(str.size()<10)return key;  
  uint16_t nlen=atoi(str.substr(0,5).c_str());
  if(str.size()<nlen+10)return key;
  key.n.set_str(str.substr(5,nlen),32);
  uint16_t xlen=atoi(str.substr(5+nlen,5).c_str());
  if(str.size()!=nlen+xlen+10)return key;  
  key.x.set_str(str.substr(10+nlen,xlen),32);
  key.str=str;
  return key;
}
void RSA::randomKey(RSA::Key&pub,RSA::Key&prv){
  clock_t time=clock();
  gmp_randstate_t grt;
  gmp_randinit_default(grt);
  gmp_randseed_ui(grt,time);
  mpz_class p,q,n,phi,e=65537,d;  
  mpz_urandomb(p.get_mpz_t(),grt,RSA_BIT_SIZE);mpz_nextprime(p.get_mpz_t(),p.get_mpz_t());
  mpz_urandomb(q.get_mpz_t(),grt,RSA_BIT_SIZE);mpz_nextprime(q.get_mpz_t(),q.get_mpz_t());
  n=p*q;
  phi=(p-1)*(q-1);
  mpz_invert(d.get_mpz_t(),e.get_mpz_t(),phi.get_mpz_t());
  prv.n=n;prv.x=d;prv.str=key2str(prv);
  pub.n=n;pub.x=e;pub.str=key2str(pub);
  
}
mpz_class RSA::crypt(const mpz_class&a,const Key&key){
  mpz_class ret;
  mpz_powm(ret.get_mpz_t(),a.get_mpz_t(),key.x.get_mpz_t(),key.n.get_mpz_t());  
  return ret;
}
