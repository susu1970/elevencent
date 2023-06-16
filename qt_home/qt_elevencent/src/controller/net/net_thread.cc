#include"net_thread.h"
#include"toast_model.h"
#include"app.h"
#include"net_protocol.hpp"
#include"net_process_load.hpp"

using namespace std;
using namespace qt_elevencent;
using namespace boost::program_options;
int NetThread::ms_maxFragPacketSize=2000;
extern string program_options2str(options_description*desc);
void NetThread::clear(){
  for(auto&out:m_outList)
    delete get<NetModel::StateCtx*>(out);
  for(auto&in:m_inList)
    delete get<NetModel::StateCtx*>(in);
  m_outList.clear();
  m_inList.clear();
  m_stateIn=NetModel::STATE_IN::START;
  m_stateOut=NetModel::STATE_OUT::START;
  m_retIn=NetModel::RETCODE::OK;
}
void NetThread::onReadyRead(){
 againRead:
  NetProcess::handleIn(this);
  if(m_retIn&NetModel::RETCODE::UNKNOWN_STATE
     ||m_retIn&NetModel::RETCODE::INVALID_PACKET){
    clear();
    m_sk->close();
    qDebug()<<"onReadyRead error";
    return;
  }
  if(m_retIn&NetModel::RETCODE::AGAIN_WRITE)
    onReadyWrite();      
  if(m_retIn&NetModel::RETCODE::AGAIN_IN)
    goto againRead;
}
void NetThread::onStateChanged(QAbstractSocket::SocketState state){}
void NetThread::pushOut(NetModel::STATE_OUT state,NetModel::StateCtx*ctx){
  m_outList.push_back(make_pair(state,ctx));
  onReadyWrite();
}
void NetThread::reconnect2server(){
  clear();
  m_sk->close();
  m_sk->waitForDisconnected();
  if(m_servers.empty()){
    App::getInstance()->postEvent(EVENT_TYPE_TOAST_SHOW,new ToastShowArg("connect to server error:\nserver host is empty!"));
    return;
  }  
  for(int trytimes=0;trytimes<m_servers.size()*4;++trytimes){
    auto iter=m_servers.begin();
    while(iter!=m_servers.end()){
      if(*iter==m_curHost){
	++iter;
	m_curHost=(iter==m_servers.end())?*m_servers.begin():*iter;
	break;
      }
      ++iter;
    }
    if(iter==m_servers.end())
      m_curHost=*m_servers.begin();
    m_sk->connectToHost(QHostAddress(QString::fromStdString(m_curHost.addr)),m_curHost.port);
    if(m_sk->waitForConnected(4000))
      break;
  }
}
ssize_t NetThread::read(char*buf,ssize_t left){
  ssize_t off=0;
  while(left>0){
    ssize_t n;
    while(left>0&&m_sk->bytesAvailable()>0&&(n=m_sk->read(buf+off,left))>0){
      left-=n;
      off+=n;
    }
    if(n==-1||m_sk->bytesAvailable()<=0)
      break;
  }
  return off;
}
ssize_t NetThread::write(char*buf,ssize_t left){
  ssize_t off=0;
  ssize_t n;
  while(left>0){
    while(left>0&&(n=m_sk->write(buf+off,left))>0){
      left-=n;
      off+=n;
    }
    if(n==-1)
      break;
  }
  m_sk->flush();
  return off;
}
void NetThread::pushServer(NetModel::Server::Host h){
  for(auto&server:m_servers)
    if(server==h)
      return;
  m_servers.push_front(h);
}
void NetThread::onReadyWrite(){
  if(m_sk->state()!=QAbstractSocket::ConnectedState)
    return;
  for(auto iter=m_outList.begin();iter!=m_outList.end();){
    auto out=*iter;
    auto ctx=get<NetModel::StateCtx*>(out);
    int left=ctx->m_dataLen-ctx->m_dealLen;
    int n=write((char*)ctx->m_data+ctx->m_dealLen,left);
    left-=n;
    ctx->m_dealLen+=n;
    if(left>0){
      m_sk->flush();
      return;
    }
    iter=m_outList.erase(iter);
    delete ctx;
  }
  m_sk->flush();
}
NetThread::NetThread(QObject*parent):QThread(parent){
  qRegisterMetaType<NetModel::Server::Host>("qt_elevencent::NetModel::Server::Host");
  qRegisterMetaType<void*>("void*");  
  qRegisterMetaType<NetModel::STATE_OUT>("qt_elevencent::NetModel::STATE_OUT");
  qRegisterMetaType<NetModel::StateCtx>("qt_elevencent::NetModel::StateCtx");  
  setParent(0);
  moveToThread(this);
  m_sk=new QTcpSocket(this);
  m_sk->moveToThread(this);
  connect(m_sk,SIGNAL(connected()),this,SLOT(onConnected()));
  connect(m_sk,SIGNAL(disconnected()),this,SLOT(onDisConnected()));
  connect(m_sk,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(onStateChanged(QAbstractSocket::SocketState)));
  connect(m_sk,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
  connect(m_sk,SIGNAL(errorOccurred(QAbstractSocket::SocketError)),this,SLOT(errorOccurred(QAbstractSocket::SocketError)));
  App::getInstance()->postEvent(EVENT_TYPE_CMD_LINE,new CmdLineModel("net server host add -a 127.0.0.1 -p 10101"));
  App::getInstance()->postEvent(EVENT_TYPE_CMD_LINE,new CmdLineModel("net server host reconnect"));
  m_timer=new QTimer(this);
  connect(m_timer,SIGNAL(timeout()),this,SLOT(timerUpdate()));
  m_timer->start(3000);
}
void NetThread::timerUpdate(){
  if(m_sk->state()!=QAbstractSocket::ConnectedState)
    App::getInstance()->postEvent(EVENT_TYPE_CMD_LINE,new CmdLineModel("net server host reconnect"));
}
void NetThread::errorOccurred(QAbstractSocket::SocketError socketError){
  qDebug()<<"errorOccurred: "<<socketError;
}
void NetThread::onConnected(){
  m_curHost.onConnected();
  QString str=(QString("success connected to server: ")+m_sk->peerName()+":"+QString::number(m_sk->peerPort()));
  App::getInstance()->postEvent(EVENT_TYPE_NET_ON_CONNECTED);  
  App::getInstance()->postEvent(EVENT_TYPE_TOAST_SHOW,new ToastShowArg(str));
  App::getInstance()->postEvent(EVENT_TYPE_CMD_LINE,new CmdLineModel("net reqpubkey"));
  App::getInstance()->postEvent(EVENT_TYPE_CMD_LINE,new CmdLineModel("net rsppubkey"));
}
void NetThread::onDisConnected(){
  QString str=(QString("disconnected to server: ")+m_sk->peerName()+":"+QString::number(m_sk->peerPort()));
  App::getInstance()->postEvent(EVENT_TYPE_NET_ON_DISCONNECTED);    
  App::getInstance()->postEvent(EVENT_TYPE_TOAST_SHOW,new ToastShowArg(str));
  m_curHost.onDisConnected();
}
NetThread::~NetThread(){
  clear();
  m_sk->close();
}
