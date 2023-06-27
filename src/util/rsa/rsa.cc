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
  *this=a;
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
RSA::Key&RSA::Key::operator=(const char*str){
  return *this=RSA::str2key(string(str));
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
string RSA::cryptWithRand16(void*binBuf,size_t len,const RSA::Key&keypub){
  using namespace std;
  int maxSegSize=(keypub.str.size()-6)/4;
  if(maxSegSize<1)
    return "";
  int segCount=len/maxSegSize+(len%maxSegSize?1:0);
  if(segCount<1)
    return "";
  string ret="";
  uint16_t urand16=((uint16_t)-1)-9999+rand()%10000;
  for(int seg=0;seg<segCount;++seg){
    mpz_class num=seg?0:urand16;
    int start=seg*maxSegSize;
    int end=MIN(len,start+maxSegSize)-1;
    while(end>=start){
      num=num*10000+1000+((uint8_t*)binBuf)[end];
      --end;
    }
    string cry=crypt(num,keypub).get_str(16);
    ret+=cry+"_";
  }
  return ret;
}
size_t RSA::decryptWithRand16(string&cryRand16,const RSA::Key&privkey,void*binBuf,size_t len){
  using namespace std;
  size_t bufIdx=0;
  int prefix=100001000;
  int counter=0;
  for(int i=0,j=0;i<cryRand16.size();++i){
    if(cryRand16[i]=='_'){
      ++counter;
      string subcry=cryRand16.substr(j,i-j);
      j=i+1;
      mpz_class num;
      num.set_str(subcry,16);
      num=crypt(num,privkey);
      while(num>=prefix){
	mpz_class mod=num%1000;
	uint8_t modui=mod.get_ui();
	if(bufIdx<len)
	  ((uint8_t*)binBuf)[bufIdx]=modui;
	++bufIdx;
	num/=10000;
      }
      prefix=1000;
    }
  }
  return bufIdx;
}
string RSA::decryptWithRand16(const string&cryRand16,const RSA::Key&privkey){
  using namespace std;
  string ret="";
  int prefix=100001000;
  for(int i=0,j=0;i<cryRand16.size();++i){
    if(cryRand16[i]=='_'){
      string subcry=cryRand16.substr(j,i-j);
      j=i+1;
      mpz_class num;
      num.set_str(subcry,16);
      num=crypt(num,privkey);
      while(num>=prefix){
	mpz_class mod=num%1000;
	uint8_t modui=mod.get_ui();
	ret.push_back(modui);
	num/=10000;
      }
      prefix=1000;
    }
  }
  return ret;
}
string RSA::cryptWithRand16(const string&plain,const RSA::Key&keypub){
  using namespace std;      
  return cryptWithRand16((void*)plain.c_str(),plain.size()+1,keypub);//end with '\0'
}
string RSA::decryptWithRand16(const char*cryRand16,const RSA::Key&privkey){
  using namespace std;      
  return decryptWithRand16(string(cryRand16),privkey);
}
size_t RSA::decryptWithRand16(const char*cryRand16,const RSA::Key&privkey,void*binBuf,size_t len){
  string str(cryRand16);
  return decryptWithRand16(str,privkey,binBuf,len);
}
