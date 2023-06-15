#include"test.h"
#include"rsa.h"

int main(int argc,char**argv){
  RSA::Key pubKey,prvKey;
  RSA::randomKey(pubKey,prvKey);
  string pubstr=RSA::key2str(pubKey);

  RSA::Key str2pubKey=RSA::str2key(pubstr);
  DEBUG_ASSERT((str2pubKey.n==pubKey.n)&&(str2pubKey.x==pubKey.x)&&(str2pubKey.str==pubKey.str),"pubKey.n: "<<pubKey.n<<"\npubKey.x: "<<pubKey.x<<"\nstr2pubKey.n: "<<str2pubKey.n<<"\nstr2pubKey.x: "<<str2pubKey.x);

  string prvstr=RSA::key2str(prvKey);
  RSA::Key str2prvKey=RSA::str2key(prvstr);
  DEBUG_ASSERT((str2prvKey.n==prvKey.n)&&(str2prvKey.x==prvKey.x)&&(str2prvKey.str==prvKey.str),"");  
  cout<<"pubKey.str: "<<pubKey.str<<endl<<"prvKey.str: "<<prvKey.str<<endl<<endl;
  while(1){
    string input;
    cin>>input;
    if(input=="q")break;
    cout<<"input is:"<<input<<endl;
    string cryped=RSA::cryptWithRand16(input,pubKey);
    cout<<"cryped is:"<<cryped<<endl;
    string decrypt=RSA::decryptWithRand16(cryped,prvKey);
    cout<<"decrypt is:"<<decrypt<<endl;
    continue;


    mpz_class plain;
    plain.set_str(input.c_str(),10);
    cout<<"input is: "<<plain<<endl;
    mpz_class c=RSA::crypt(plain,pubKey);
    cout<<"pub cypher: "<<c<<endl;
    mpz_class plainDec=RSA::crypt(c,prvKey);
    cout<<"prv plainDec: "<<plainDec<<endl;
    c=RSA::crypt(plain,prvKey);
    cout<<"prv cypher: "<<c<<endl;
    plainDec=RSA::crypt(c,pubKey);
    cout<<"pub plainDec: "<<plainDec<<endl;
  }
}
