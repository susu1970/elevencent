#ifndef _ELEVENCENT_RSA_H_
#define _ELEVENCENT_RSA_H_

#include<stdint.h>
#include<gmpxx.h>
#include<vector>

namespace elevencent{
  class RSA;
  class RSA{
  public:
    class Key{
    public:
      mpz_t n;
      mpz_t x;
      static void randomKey(Key&pub,Key&prv);
      Key();
      Key(const Key&a);
      Key&operator=(const Key&a);
      ~Key();
    };
    static void crypt(const mpz_t&a,mpz_t&b,const Key&key);
  };
  std::ostream&operator<<(std::ostream&out,const RSA::Key&a){
    gmp_printf("\n------<RSA::Key>-----\nn: %Zd\nx: %Zd\n------<RSA::Key>-----\n",a.n,a.x);
    return out;
  }

}

#endif
