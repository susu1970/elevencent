#include"test.h"
#include"mariadb_resource.h"
#include<sstream>

void hintMsg(){
  cout<<"h help\n"
    "q: quit\n"
    "1: insert passwd resource\n"
    "2: insert user resource with id\n"
    "3: insert user resource\n"
    "4: insert passwd resource with id\n"
    "5: insert passwd resource\n"
    "6: user ref resource\n"        
    ;
}
int main(int argc,char**argv){
  if(argc!=2){
    cerr<<"usage: <"<<argv[0]<<"> <ini-path>"<<endl;
    exit(1);
  }
  string iniFile=argv[1];
  MariadbResource*mr=new MariadbResource(iniFile);
  cout<<"input cmd(h for help)..."<<endl;
  string cmd="";
  stringstream   ss;
  while(getline(cin,cmd)&&cmd!="q"){//receive the command from terminal
    if(cmd=="h"){
      hintMsg();
      continue;
    }else if(cmd=="1"){
      cout<<"input passwd:";
      string passwd;
      getline(cin,passwd);
      resource_id_t id;
    }else if(cmd=="2"){
      stringstream ss;
      cout<<"input userResourceId:";
      string input;
      getline(cin,input);
      resource_id_t id=0;
      ss<<input;
      ss>>id;
      ss.clear();
      bool res=mr->insertUserResource(id);
      cout<<"res: "<<res<<", userResourceId: "<<id<<endl;
    }else if(cmd=="3"){
      resource_id_t id;
      bool res=mr->insertUserResource(&id);
      cout<<"res: "<<res<<", userResourceId: "<<id<<endl;
    }else if(cmd=="4"){
      stringstream ss;
      cout<<"input passwdResourceId:";
      string input;
      getline(cin,input);
      resource_id_t id=0;
      ss<<input;
      ss>>id;
      ss.clear();
      string passwd;
      cout<<"input passwd:";
      getline(cin,passwd);
      bool res=mr->insertPasswdResource(passwd,id);
      cout<<"res: "<<res<<", passwdResourceId: "<<id<<", passwd: "<<endl;      
    }else if(cmd=="5"){
      string passwd;
      cout<<"input passwd:";
      resource_id_t id;
      getline(cin,passwd);
      bool res=mr->insertPasswdResource(passwd,&id);
      cout<<"res: "<<res<<", passwdResourceId: "<<id<<", passwd: "<<endl;            
    }else if(cmd=="6"){
      stringstream ss;
      cout<<"input user resource id:";
      string input;
      getline(cin,input);
      resource_id_t uid=0;
      ss<<input;
      ss>>uid;
      ss.clear();
      cout<<"input resource id:";
      getline(cin,input);
      resource_id_t rid=0;
      ss<<input;
      ss>>rid;
      ss.clear();
      bool res=mr->userRefResource(uid,rid,19700101);
      cout<<"res: "<<res<<endl;
    }
  }
  return 0;
}

