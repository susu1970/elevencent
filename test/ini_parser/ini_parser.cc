#include"test.h"
#include<INIReader.h>


int main(int argc,char**argv){
  if(argc!=4)
    cerr<<"usage: "<<argv[0]<<" file section name defaultValue"<<endl,exit(1);
  string f1=argv[1];
  string section=argv[2];
  string name=argv[3];
  INIReader ini(f1);
  cout<<"ini.GetString: \""<<ini.GetString(section,name,"")<<"\""<<endl;
  return 0;
}
