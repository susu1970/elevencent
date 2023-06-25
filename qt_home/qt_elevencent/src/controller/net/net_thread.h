#ifndef NET_THREAD_H_
#define NET_THREAD_H_
#include"net_model.h"
#include<list>
#include<QTcpSocket>
#include<QThread>
#include<unordered_set>
#include<QMutex>
#include<QMetaType>

namespace qt_elevencent{
  class NetThread:public QThread{
    Q_OBJECT
  public:
    explicit NetThread(QObject*parent=0);
    ~NetThread();
  private:
    void clear();
    QTimer*m_timer;
  private slots:
    void timerUpdate();
  public:
    QTcpSocket*m_sk;
    NetModel::STATE_IN m_stateIn=NetModel::STATE_IN::START;
    NetModel::STATE_OUT m_stateOut=NetModel::STATE_OUT::START;
    int m_retIn=NetModel::RETCODE::OK;
    std::list<std::pair<NetModel::STATE_OUT,NetModel::StateCtx*>>m_outList;
    std::list<std::pair<NetModel::STATE_IN,NetModel::StateCtx*>>m_inList;
    std::unordered_map<uint32_t,std::pair<void*,std::function<void(void*)>>>m_fragPacketCtx;
    std::list<NetModel::Server::Host>m_servers;
    NetModel::Server::Host m_curHost;
    static int ms_maxFragPacketSize;
  public:
    ssize_t write(char*buf,ssize_t len);
    ssize_t read(char*buf,ssize_t len);

  public slots:
    void onReadyWrite();
    void pushOut(NetModel::STATE_OUT state,NetModel::StateCtx*);
    void pushServer(NetModel::Server::Host h);    
    void onConnected();
    void onDisConnected();
    void onStateChanged(QAbstractSocket::SocketState);
    void errorOccurred(QAbstractSocket::SocketError socketError);
    void onReadyRead();
    void reconnect2server();
  };
}

#endif
