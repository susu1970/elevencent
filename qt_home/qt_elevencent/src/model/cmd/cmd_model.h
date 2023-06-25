#ifndef CMD_MODEL_H_
#define CMD_MODEL_H_
#include<QString>
#include<string>

namespace qt_elevencent{
  #define MAX_CMD_ARGC 512
  #define MAX_CMD_ARGV 1024
  #define MAX_CMD_PRINT 1024

  enum class CMD_VIEW_CONTENT_IDX{
    HINT,
    CONTENT,
  };
  class CmdViewOutputArg{
  public:
    CMD_VIEW_CONTENT_IDX m_target;
    QString m_text;
    bool m_append;
    QString m_preFormat,m_tailFormat;
    CmdViewOutputArg(CMD_VIEW_CONTENT_IDX target,QString&&text,bool append=true,QString&&preFormat="",QString&&tailFormat=""):m_target(target),m_text(std::forward<QString>(text)),m_preFormat(std::forward<QString>(preFormat)),m_tailFormat(std::forward<QString>(tailFormat)),m_append(append){}
    CmdViewOutputArg(CMD_VIEW_CONTENT_IDX target,QString&text,bool append=true,QString&&preFormat="",QString&&tailFormat=""):m_target(target),m_text(std::forward<QString>(text)),m_preFormat(std::forward<QString>(preFormat)),m_tailFormat(std::forward<QString>(tailFormat)),m_append(append){}    
  };
  class CmdModel{
  public:
    int argc=0;
    char*argv[MAX_CMD_ARGC];
    CmdModel(){
      for(int i=0;i<MAX_CMD_ARGC;++i)
	argv[i]=new char[MAX_CMD_ARGV];	
    }
    ~CmdModel(){
      for(int i=0;i<MAX_CMD_ARGC;++i)
	delete[]argv[i];
    }
  };
  class CmdLineModel{
  public:
    QString cmdline;
    CmdLineModel(QString cmdline_):cmdline(cmdline_){}
  };
}

#endif
