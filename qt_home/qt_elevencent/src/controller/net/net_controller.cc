#include"net_controller.h"
#include"toast_model.h"
#include"string.h"
#include"app.h"
#include"net_protocol.hpp"
#include"net_process_load.hpp"
using namespace std;
using namespace qt_elevencent;
using namespace boost::program_options;
extern string program_options2str(options_description*desc);
static unordered_map<string,TcpProtocol::UserSendMsg::MSG_TYPE>s_str2msgType={
  {"string",TcpProtocol::UserSendMsg::MSG_TYPE::STRING}
};
NetController::NetController(QObject*parent):BaseController(parent){
  connect(this,SIGNAL(reconnect2server()),&m_thr,SLOT(reconnect2server()));
  connect(this,SIGNAL(pushServer(NetModel::Server::Host)),&m_thr,SLOT(pushServer(NetModel::Server::Host)));
  connect(this,SIGNAL(pushOut(NetModel::STATE_OUT,NetModel::StateCtx*)),&m_thr,SLOT(pushOut(NetModel::STATE_OUT,NetModel::StateCtx*)));  
  m_thr.start();  
  registeCmd({"net","reqpubkey"},[](int argc,char**argv,int match,void*pctl){
    NetController*ctl=(NetController*)pctl;
    if(!ctl)return;
    TcpProtocol::Head*head=new TcpProtocol::Head(TcpProtocol::Head::TYPE::REQUEST_PUBKEY);
    head->h2n();
    emit ctl->pushOut(NetModel::STATE_OUT::REQUEST_PUBKEY,new NetModel::StateCtx(head,sizeof(TcpProtocol::Head),[](NetModel::StateCtx*ctx){
      delete (TcpProtocol::Head*)ctx->m_data;
    }));
  });
  registeCmd({"net","rsppubkey"},[](int argc,char**argv,int match,void*pctl){
    NetController*ctl=(NetController*)pctl;
    if(!ctl||g_pubkey.empty())return;
    TcpProtocol::RsaPubkey*pubkey=(TcpProtocol::RsaPubkey*)(new char[g_pubkey.size()+sizeof(TcpProtocol::RsaPubkey)]);
    pubkey->type=TcpProtocol::Head::TYPE::RESP_PUBKEY;
    pubkey->len=g_pubkey.size();
    memcpy((char*)(pubkey+1),g_pubkey.c_str(),g_pubkey.size());
    pubkey->h2n();
    emit ctl->pushOut(NetModel::STATE_OUT::RESP_PUBKEY,new NetModel::StateCtx(pubkey,sizeof(TcpProtocol::RsaPubkey)+g_pubkey.size(),[](NetModel::StateCtx*ctx){
      delete []((char*)ctx->m_data);
    }));
  });
  registeCmd({"net","server","host","show"},[](int argc,char**argv,int match,void*pctl){
    NetController*ctl=(NetController*)pctl;
    if(!ctl)return;
    CmdViewOutputArg outArg(CMD_VIEW_CONTENT_IDX::CONTENT,"server list:\n");
    App::getInstance()->sendEvent(EVENT_TYPE_CMD_VIEW_OUTPUT,&outArg);
    for(auto&server:ctl->m_thr.m_servers){
      outArg.m_text=QString::fromStdString(server.str());
      App::getInstance()->sendEvent(EVENT_TYPE_CMD_VIEW_OUTPUT,&outArg);
    }
  });
  shared_ptr<options_description>reqmsg_desc=make_shared<options_description>("req msg from user\n");
  reqmsg_desc->add_options()
    ("name,n",value<string>(),"user name")
    ("from,f",value<uint64_t>()->default_value(0),"date from")
    ("to,t",value<uint64_t>()->default_value(-1ul),"data to");
  registeCmd({"net","user_resource","reqmsg"},[](int argc,char**argv,int match,void*pctl){
    NetModel::Server::Host curhost;
    App::getInstance()->sendEvent(EVENT_TYPE_GET_CONNED_SERVER,&curhost);    
    if(!curhost.isConn){
      ToastShowArg arg("server host not connected\n");
      App::getInstance()->sendEvent(EVENT_TYPE_TOAST_SHOW,&arg);      
      return;
    }
    if(curhost.rsaKeypub.str.empty()){
      ToastShowArg arg("server's rsa pubkey empty\n");
      App::getInstance()->sendEvent(EVENT_TYPE_TOAST_SHOW,&arg);
      return;
    }
    if(!curhost.isLogin){
      ToastShowArg arg("not login\n");
      App::getInstance()->sendEvent(EVENT_TYPE_TOAST_SHOW,&arg);
      return;      
    }
    NetController*ctl=(NetController*)pctl;
    if(!ctl)return;
    const BaseController::CmdFuncPair*pair=ctl->getCmdPair(argc,argv,match);
    auto desc=pair->desc;
    if(!desc)return;
    variables_map vm;
    string help=program_options2str(desc.get());
    try{
      store(parse_command_line(argc,argv,*desc),vm);
    }catch(const error&ex){
      CmdViewOutputArg outArg(CMD_VIEW_CONTENT_IDX::HINT,QString::fromStdString(ex.what()+string("\n")+help+"\n"));
      App::getInstance()->sendEvent(EVENT_TYPE_CMD_VIEW_OUTPUT,&outArg);      
      return;
    }
    if(!vm.count("name")){
      CmdViewOutputArg outArg(CMD_VIEW_CONTENT_IDX::HINT,QString::fromStdString(string("invalid argument \n")+help));
      App::getInstance()->sendEvent(EVENT_TYPE_CMD_VIEW_OUTPUT,&outArg);
      return;
    }
    char binBuf[5192]={0};
    TcpProtocol::UserReqMsg::CryContent*cryptContent=(TcpProtocol::UserReqMsg::CryContent*)binBuf;
    cryptContent->token=curhost.loginToken;
    cryptContent->from=vm["from"].as<uint64_t>();
    cryptContent->to=vm["to"].as<uint64_t>();
    cryptContent->nameLen=vm["name"].as<string>().size();
    char*name=(char*)(cryptContent+1);
    if(name+vm["name"].as<string>().size()+1>=binBuf+sizeof(binBuf)){
      ToastShowArg arg("user req msg error: name too long \n");
      App::getInstance()->sendEvent(EVENT_TYPE_TOAST_SHOW,&arg);   
      return;      
    }
    strcpy(name,vm["name"].as<string>().c_str());
    cryptContent->hton();
    string crypt=RSA::cryptWithRand16(binBuf,sizeof(TcpProtocol::UserReqMsg::CryContent)+vm["name"].as<string>().size()+1,curhost.rsaKeypub);
    if(crypt.empty()){
      ToastShowArg arg("user req msg error, crypt error:\nmaybe length too long \n");
      App::getInstance()->sendEvent(EVENT_TYPE_TOAST_SHOW,&arg);   
      return;            
    }
    TcpProtocol::UserReqMsg*reqmsg=(TcpProtocol::UserReqMsg*)new char[sizeof(TcpProtocol::UserReqMsg)+crypt.size()];
    reqmsg->type=TcpProtocol::Head::TYPE::USER_REQ_MSG;
    reqmsg->cryptlen=crypt.size();
    memcpy(reqmsg+1,crypt.c_str(),crypt.size());
    reqmsg->h2n();
    emit ctl->pushOut(NetModel::STATE_OUT::USER_REQ_MSG,new NetModel::StateCtx(reqmsg,sizeof(TcpProtocol::UserReqMsg)+crypt.size(),[](NetModel::StateCtx*ctx){
      delete[](char*)ctx->m_data;
    }));    
  },reqmsg_desc);
  
  shared_ptr<options_description>reqmsg_users_desc=make_shared<options_description>("req msg users\n");
  registeCmd({"net","user_resource","reqmsg_users"},[](int argc,char**argv,int match,void*pctl){
    NetModel::Server::Host curhost;
    App::getInstance()->sendEvent(EVENT_TYPE_GET_CONNED_SERVER,&curhost);    
    if(!curhost.isConn){
      return;
    }
    if(curhost.rsaKeypub.str.empty()){
      return;
    }
    if(!curhost.isLogin){
      return;      
    }
    NetController*ctl=(NetController*)pctl;
    if(!ctl)return;
    const BaseController::CmdFuncPair*pair=ctl->getCmdPair(argc,argv,match);
    auto desc=pair->desc;
    if(!desc)return;
    variables_map vm;
    string help=program_options2str(desc.get());
    try{
      store(parse_command_line(argc,argv,*desc),vm);
    }catch(const error&ex){
      CmdViewOutputArg outArg(CMD_VIEW_CONTENT_IDX::HINT,QString::fromStdString(ex.what()+string("\n")+help+"\n"));
      App::getInstance()->sendEvent(EVENT_TYPE_CMD_VIEW_OUTPUT,&outArg);      
      return;
    }
    TcpProtocol::RespLogin::Token token=curhost.loginToken;
    token.hton();
    string crypt=RSA::cryptWithRand16(&token,sizeof(token),curhost.rsaKeypub);
    if(crypt.empty()){
      return;            
    }
    TcpProtocol::ReqMsgUsers*req=(TcpProtocol::ReqMsgUsers*)new char[sizeof(TcpProtocol::ReqMsgUsers)+crypt.size()];
    req->type=TcpProtocol::Head::TYPE::REQ_MSG_USERS;
    req->cryptlen=crypt.size();
    memcpy(req+1,crypt.c_str(),crypt.size());
    req->h2n();
    emit ctl->pushOut(NetModel::STATE_OUT::REQ_MSG_USERS,new NetModel::StateCtx(req,sizeof(TcpProtocol::ReqMsgUsers)+crypt.size(),[](NetModel::StateCtx*ctx){
      delete[](char*)ctx->m_data;
    }));    
  },reqmsg_users_desc);

  shared_ptr<options_description>sendmsg_desc=make_shared<options_description>("send msg to user\n");
  sendmsg_desc->add_options()
    ("name,n",value<string>(),"user name")
    ("type,t",value<string>()->default_value("string"),"msg type(string)")
    ("msg,m",value<string>(),"msg");
  registeCmd({"net","user_resource","sendmsg"},[](int argc,char**argv,int match,void*pctl){
    NetModel::Server::Host curhost;
    App::getInstance()->sendEvent(EVENT_TYPE_GET_CONNED_SERVER,&curhost);    
    if(!curhost.isConn){
      ToastShowArg arg("server host not connected\n");
      App::getInstance()->sendEvent(EVENT_TYPE_TOAST_SHOW,&arg);      
      return;
    }
    if(curhost.rsaKeypub.str.empty()){
      ToastShowArg arg("server's rsa pubkey empty\n");
      App::getInstance()->sendEvent(EVENT_TYPE_TOAST_SHOW,&arg);
      return;
    }
    if(!curhost.isLogin){
      ToastShowArg arg("not login\n");
      App::getInstance()->sendEvent(EVENT_TYPE_TOAST_SHOW,&arg);
      return;      
    }
    NetController*ctl=(NetController*)pctl;
    if(!ctl)return;
    const BaseController::CmdFuncPair*pair=ctl->getCmdPair(argc,argv,match);
    auto desc=pair->desc;
    if(!desc)return;
    variables_map vm;
    string help=program_options2str(desc.get());
    try{
      store(parse_command_line(argc,argv,*desc),vm);
    }catch(const error&ex){
      CmdViewOutputArg outArg(CMD_VIEW_CONTENT_IDX::HINT,QString::fromStdString(ex.what()+string("\n")+help+"\n"));
      App::getInstance()->sendEvent(EVENT_TYPE_CMD_VIEW_OUTPUT,&outArg);      
      return;
    }
    if(!vm.count("name")||!vm.count("msg")||s_str2msgType.find(vm["type"].as<string>())==s_str2msgType.end()){
      CmdViewOutputArg outArg(CMD_VIEW_CONTENT_IDX::HINT,QString::fromStdString(string("invalid argument \n")+help));
      App::getInstance()->sendEvent(EVENT_TYPE_CMD_VIEW_OUTPUT,&outArg);
      return;
    }
    char binBuf[10240]={0};
    TcpProtocol::RespLogin::Token*token=(TcpProtocol::RespLogin::Token*)binBuf;
    *token=curhost.loginToken;
    token->hton();
    char*name=(char*)(token+1);
    char*msg=name+vm["name"].as<string>().size()+1;
    if(msg+vm["msg"].as<string>().size()+1>=binBuf+sizeof(binBuf)){
      ToastShowArg arg("user send msg error:\nmsg too long \n");
      App::getInstance()->sendEvent(EVENT_TYPE_TOAST_SHOW,&arg);   
      return;      
    }
    strcpy(name,vm["name"].as<string>().c_str());
    strcpy(msg,vm["msg"].as<string>().c_str());
    string crypt=RSA::cryptWithRand16(binBuf,sizeof(TcpProtocol::RespLogin::Token)+vm["name"].as<string>().size()+1+vm["msg"].as<string>().size(),curhost.rsaKeypub);
    if(crypt.empty()){
      ToastShowArg arg("user send msg error, crypt error:\nmaybe length too long \n");
      App::getInstance()->sendEvent(EVENT_TYPE_TOAST_SHOW,&arg);   
      return;            
    }
    TcpProtocol::UserSendMsg*sndmsg=(TcpProtocol::UserSendMsg*)new char[sizeof(TcpProtocol::UserSendMsg)+crypt.size()];
    sndmsg->cryptlen=crypt.size();
    sndmsg->type=TcpProtocol::Head::TYPE::USER_SEND_MSG;
    sndmsg->msgType=s_str2msgType[vm["type"].as<string>()];
    sndmsg->nameLen=vm["name"].as<string>().size();
    sndmsg->msgLen=vm["msg"].as<string>().size();
    memcpy(sndmsg+1,crypt.c_str(),crypt.size());
    sndmsg->h2n();
    emit ctl->pushOut(NetModel::STATE_OUT::USER_SEND_MSG,new NetModel::StateCtx(sndmsg,sizeof(TcpProtocol::UserSendMsg)+crypt.size(),[](NetModel::StateCtx*ctx){
      delete[](char*)ctx->m_data;
    }));    
  },sendmsg_desc);  
  shared_ptr<options_description>host_set_desc=make_shared<options_description>("set server host\n");
  host_set_desc->add_options()
    ("addr,a",value<string>(),"address")
    ("port,p",value<uint16_t>(),"port");
  registeCmd({"net","server","host","add"},[](int argc,char**argv,int match,void*pctl){
    int sub_argc=argc-match+1;
    NetController*ctl=(NetController*)pctl;
    if(sub_argc<=0||!ctl||match<1)
      return;
    char**sub_argv=&(argv[match-1]);
    const BaseController::CmdFuncPair*pair=ctl->getCmdPair(argc,argv,match);
    if(!pair)return;
    auto desc=pair->desc;
    if(!desc)return;
    variables_map vm;
    string help=program_options2str(desc.get());
    try{
      store(parse_command_line(sub_argc,sub_argv,*desc),vm);
    }catch(const error&ex){
      CmdViewOutputArg outArg(CMD_VIEW_CONTENT_IDX::HINT,QString::fromStdString(string("set server host error:\n")+ex.what()+string("\n")+help+"\n"));
      App::getInstance()->sendEvent(EVENT_TYPE_CMD_VIEW_OUTPUT,&outArg);
      return;
    }
    string addr=vm.count("addr")?vm["addr"].as<string>():"";
    if(addr.empty()){
      CmdViewOutputArg outArg(CMD_VIEW_CONTENT_IDX::HINT,QString::fromStdString("server address should not be empty:\n"+help+"\n"));
      App::getInstance()->sendEvent(EVENT_TYPE_CMD_VIEW_OUTPUT,&outArg);
      return;
    }
    uint16_t port=vm.count("port")?vm["port"].as<uint16_t>():0;
    emit ctl->pushServer(NetModel::Server::Host(addr,port));
  },host_set_desc);
  registeCmd({"net","server","host","reconnect"},[](int argc,char**argv,int match,void*pctl){
    NetController*ctl=(NetController*)pctl;
    if(!ctl)return;
    emit ctl->reconnect2server();
  });

  
}
NetController::~NetController(){
  m_thr.quit();
  m_thr.wait();
}
bool NetController::appEvent(AppEvent*ev){
  switch(ev->type()){
  case EVENT_TYPE_NET_REQ_PUBKEY:{
    TcpProtocol::Head*head=new TcpProtocol::Head(TcpProtocol::Head::TYPE::REQUEST_PUBKEY);
    head->h2n();
    emit pushOut(NetModel::STATE_OUT::REQUEST_PUBKEY,new NetModel::StateCtx(head,sizeof(TcpProtocol::Head),[](NetModel::StateCtx*ctx){
      delete (TcpProtocol::Head*)ctx->m_data;
    }));
    return true;
  }
    break;
  case EVENT_TYPE_NET_TX:{
    NetModel::Tx*tx=(NetModel::Tx*)ev->data;
    emit pushOut(tx->m_state,tx->m_ctx);
    return true;
  }
    break;
  case EVENT_TYPE_GET_CONNED_SERVER:{
    NetModel::Server::Host*host=(NetModel::Server::Host*)ev->data;
    *host=m_thr.m_curHost;
    return true;
  }
    break;
  default:
    break;
  }
  return false;
}
