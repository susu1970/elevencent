#include"test.h"
#include"rsa.h"

int main(int argc,char**argv){
  RSA::Key pubKey,prvKey;
  RSA::Key::randomKey(pubKey,prvKey);
  DEBUG_PRETTY_MSG("input plain text: ");
  while(1){
    string input;
    cin>>input;
    if(input=="q")break;
    mpz_t plain;
    mpz_init_set_str(plain,input.c_str(),10);
    gmp_printf("plain is: %Zd\n",plain);
    mpz_t c;
    mpz_init(c);
    mpz_t  plainDec;
    mpz_init(plainDec);
    RSA::crypt(plain,c,pubKey);
    gmp_printf("pub cypher is: %Zd\n",c);
    RSA::crypt(c,plainDec,prvKey);
    gmp_printf("prv plainDec is: %Zd\n",plainDec);
    RSA::crypt(plain,c,prvKey);
    gmp_printf("prv cypher is: %Zd\n",c);
    RSA::crypt(c,plainDec,pubKey);
    gmp_printf("pub plainDec is: %Zd\n",plainDec);  
    
  }
}
