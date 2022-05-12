#include"test.h"
#include"rsa.h"

int main(int argc,char**argv){
  RSA::Key pubKey,prvKey;
  RSA::randomKey(pubKey,prvKey);
  string pubstr=RSA::key2str(pubKey);
  cout<<"pubKey.str: "<<pubKey.str<<endl<<endl;
  cout<<"pubstr: "<<pubstr<<endl<<endl;  
  RSA::Key str2pubKey=RSA::str2key(pubstr);
  DEBUG_ASSERT((str2pubKey.n==pubKey.n)&&(str2pubKey.x==pubKey.x)&&(str2pubKey.str==pubKey.str),"pubKey.n: "<<pubKey.n<<"\npubKey.x: "<<pubKey.x<<"\nstr2pubKey.n: "<<str2pubKey.n<<"\nstr2pubKey.x: "<<str2pubKey.x);

  string prvstr=RSA::key2str(prvKey);
  cout<<"prvKey.str: "<<prvstr<<endl<<endl;
  cout<<"prvstr: "<<prvstr<<endl<<endl;
  RSA::Key str2prvKey=RSA::str2key(prvstr);
  DEBUG_ASSERT((str2prvKey.n==prvKey.n)&&(str2prvKey.x==prvKey.x)&&(str2prvKey.str==prvKey.str),"");  
  while(1){
    string input;
    cin>>input;
    if(input=="q")break;
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
