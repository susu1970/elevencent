#include"test.h"
#include"mariadb_resource.h"
#include<sstream>

void hintMsg(){
  cout<<"h help\ninsert passwdResource(1) <passwd>\ninsert userResource(2) <userResourceId,userResourceMask>"<<endl;
}
int main(int argc,char**argv){
  if(argc!=2){
    cerr<<"usage: <"<<argv[0]<<"> <ini-path>"<<endl;
    exit(1);
  }
  string iniFile=argv[1];
  MariadbResource*mr=new MariadbResource(iniFile);
  cout<<"input cmd..."<<endl;
  string cmd="";
  
  while(getline(cin,cmd)&&cmd!="q"){//receive the command from terminal
    if(cmd=="h"){
      hintMsg();
      continue;
    }else if(cmd=="1"||cmd.find("insert passwdResource")!=string::npos){
      cout<<"input passwd:";
      string passwd;
      getline(cin,passwd);
      resource_id_t id;
      bool res=mr->insertPasswdResource(&id,passwd);
      cout<<"res: "<<res<<", passwdResourceId: "<<id<<", passwd: "<<passwd<<endl;
    }else if(cmd=="2"||cmd.find("insert userResource")!=string::npos){
      stringstream ss;
      cout<<"input userResourceId:";
      string input;
      getline(cin,input);
      resource_id_t id=0;
      ss<<input;
      ss>>id;
      ss.clear();
      cout<<"input userResourceMask:";
      getline(cin,input);
      resource_mask_t mask=0;
      ss<<input;
      ss>>mask;
      ss.clear();      
      bool res=mr->insertUserResource(id,mask);
      cout<<"res: "<<res<<", userResourceId: "<<id<<", userResourceMask: "<<mask<<endl;
    }else if(cmd=="3"||cmd.find("insert userResourceWithPasswd")!=string::npos){
      cout<<"input userResourceId:";
      string id;
      getline(cin,id);
      string passwd;
      cout<<"input passwd:";
      getline(cin,passwd);
      resource_id_t uid;
      ss<<id;
      ss>>uid;
      ss.clear();
      bool res=mr->insertUserResource(uid,0,passwd,);
    }
  }
  return 0;
}

