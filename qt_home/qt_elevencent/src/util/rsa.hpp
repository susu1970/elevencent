#ifndef RSA_HPP_
#define RSA_HPP_
#include"global.h"
#include<gmpxx.h>
#include<string>
#include<iostream>
#include<ctime>  
#include<cstdlib>
#include<QDebug>
#include<gmpxx.h>
#include<vector>
#include<string>
#include<time.h>
#include<stdlib.h>
#include<iostream>

namespace qt_elevencent{
#define RSA_BIT_SIZE 2048      
  class RSA{
  public:
    class Key{
    public:
      mpz_class n;
      mpz_class x;
      std::string str="";
      Key(){}
      Key(const Key&a){
	if(this!=&a){
	  n=a.n;
	  x=a.x;
	  str=a.str;
	}	
      }
      Key(const std::string&str){
	*this=RSA::str2key(str);	
      }
      Key&operator=(const Key&a){
	if(this!=&a){
	  n=a.n;
	  x=a.x;
	  str=a.str;
	}
	return *this;
      }
      Key&operator=(const std::string&str){
	return *this=RSA::str2key(str);	
      }
      Key&operator=(const char*str){
	return *this=str2key(std::string(str));
      }
    };
    static void randomKey(Key&pub,Key&prv){
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
    static mpz_class crypt(const mpz_class&a,const std::string&key){
      return crypt(a,str2key(key));
    }
    static mpz_class crypt(const mpz_class&a,const Key&key){
      mpz_class ret;
      mpz_powm(ret.get_mpz_t(),a.get_mpz_t(),key.x.get_mpz_t(),key.n.get_mpz_t());  
      return ret;
    }
    static Key str2key(const std::string&str){
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
    static std::string key2str(const Key&key){
      using namespace std;
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
    static std::string cryptWithRand16(const std::string&plain,const Key&keypub){
      using namespace std;
      srand(time(nullptr));
      mpz_class num=((uint16_t)-1)-9999+rand()%10000;
      for(int i=(int)plain.size()-1;i>=0;--i)
	num=num*10000+1000+((uint8_t)plain[i]);
      string str=num.get_str();
      if(str.size()>=keypub.str.size()||str.size()<1)
	return "";
      num.set_str(str,10);
      return crypt(num,keypub).get_str();
    }
    static std::string decryptWithRand16(const char*cryRand16,const Key&privkey){
      using namespace std;      
      mpz_class num;
      num.set_str(cryRand16,10);
      num=crypt(num,privkey);
      string str=num.get_str();
      if(str.size()<5)
	return "";
      int i=0;
      while(num>100000000){
	mpz_class mod=num%1000;
	uint8_t modui=mod.get_ui();
	str[i++]=modui;
	num/=10000;
      }
      for(;i<str.size();++i)
	str[i]=0;
      return str;
    }
    static std::string decryptWithRand16(const std::string&cryRand16,const Key&privkey){
      return decryptWithRand16(cryRand16.c_str(),privkey);
    }
    static std::string cryptWithRand16(void*binBuf,size_t len,const RSA::Key&keypub){
      using namespace std;
      srand(time(nullptr));
      mpz_class num=((uint16_t)-1)-9999+rand()%10000;
      uint8_t*p=(uint8_t*)binBuf;
      for(int i=len-1;i>=0;--i)
	num=num*10000+1000+p[i];
      string str=num.get_str();
      if(str.size()>=keypub.str.size())
	return "";
      num.set_str(str,10);
      return crypt(num,keypub).get_str();
    }
    static size_t decryptWithRand16(const char*cryRand16,const RSA::Key&privkey,void*binBuf,size_t len){
      using namespace std;
      mpz_class num;
      num.set_str(cryRand16,10);
      num=crypt(num,privkey);
      uint8_t*p=(uint8_t*)binBuf;  
      size_t i=0;
      while(num>100000000){
	mpz_class mod=num%1000;
	uint8_t modui=mod.get_ui();
	if(i<len)
	  p[i]=modui;
	++i;
	num/=10000;
      }
      return i;
    }    
  };
  extern RSA::Key g_keypub;
  extern RSA::Key g_keypriv;
}


#endif
