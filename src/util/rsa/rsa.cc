#include"rsa.h"

#include<time.h>
#include<iostream>
using namespace std;
using namespace elevencent;

#define RSA_BIT_SIZE 2048

RSA::Key&RSA::Key::operator=(const RSA::Key&a){
  if(this!=&a){
    mpz_set(n,a.n);
    mpz_set(x,a.x);
  }
  return *this;
}
RSA::Key::Key(){
  mpz_init(n);
  mpz_init(x);
}
RSA::Key::~Key(){
  mpz_clear(n);
  mpz_clear(x);
}
void RSA::Key::randomKey(RSA::Key&pub,RSA::Key&prv){
  clock_t time=clock();
  gmp_randstate_t grt;
  gmp_randinit_default(grt);
  gmp_randseed_ui(grt,time);
  mpz_t p,q,n,phi,e,d;  
  mpz_init(p);mpz_init(q);mpz_init(n);mpz_init(phi);mpz_init_set_ui(e,65537);mpz_init(d);
  mpz_urandomb(p,grt,RSA_BIT_SIZE);mpz_nextprime(p,p);
  mpz_urandomb(q,grt,RSA_BIT_SIZE);mpz_nextprime(q,q);
  mpz_mul(n,p,q);
  mpz_sub_ui(p,p,1);
  mpz_sub_ui(q,q,1);
  mpz_mul(phi,p,q);
  mpz_invert(d,e,phi);
  mpz_set(prv.n,n);
  mpz_set(prv.x,d);  
  mpz_set(pub.n,n);
  mpz_set(pub.x,e);  
  mpz_clear(p);mpz_clear(q);mpz_clear(n);mpz_clear(phi);mpz_clear(e);mpz_clear(d);  
}
void RSA::crypt(const mpz_t&a,mpz_t&b,const Key&key){
  mpz_powm(b,a,key.x,key.n);
}
