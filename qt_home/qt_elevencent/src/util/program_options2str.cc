#include<boost/program_options.hpp>
#include<iostream>
#include<memory>
#include<sstream>
using namespace std;
using namespace boost::program_options;
string program_options2str(options_description*desc){
  if(!desc)return "";
  stringstream ss;
  streambuf*out_buf=cout.rdbuf();
  cout.rdbuf(ss.rdbuf());
  cout<<*desc;
  string str(ss.str());
  ss.clear();
  cout.rdbuf(out_buf);
  return str;
}
