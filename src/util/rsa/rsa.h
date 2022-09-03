#ifndef _ELEVENCENT_RSA_H_
#define _ELEVENCENT_RSA_H_

#include<stdint.h>
#include<gmpxx.h>
#include<vector>
#include<string>

namespace elevencent{
  class RSA{
  public:
    class Key{
    public:
      mpz_class n;
      mpz_class x;
      std::string str="";
      Key();
      Key(const Key&a);
      Key(const std::string&str);
      Key&operator=(const Key&a);
      Key&operator=(const std::string&str);      
    };
    static void randomKey(Key&pub,Key&prv);    
    static mpz_class crypt(const mpz_class&a,const Key&key);
    static Key str2key(const std::string&str);
    static std::string key2str(const Key&key);
  };
}

#endif
