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
      Key&operator=(const char*str);      
    };
    static void randomKey(Key&pub,Key&prv);    
    static mpz_class crypt(const mpz_class&a,const Key&key);
    static Key str2key(const std::string&str);
    static std::string key2str(const Key&key);
    static std::string decryptWithRand16(const std::string&cryRand16,const RSA::Key&privkey);
    static std::string decryptWithRand16(const char*cryRand16,const RSA::Key&privkey);
    static std::string cryptWithRand16(const std::string&plain,const RSA::Key&keypub);

    static std::string cryptWithRand16(void*binBuf,size_t len,const RSA::Key&keypub);
    static size_t decryptWithRand16(const char*cryRand16,const RSA::Key&privkey,void*binBuf,size_t len);
    static size_t decryptWithRand16(std::string&cryRand16,const RSA::Key&privkey,void*binBuf,size_t len);    

  };
  extern RSA::Key g_keypub;
  extern RSA::Key g_keypriv;
}

#endif
