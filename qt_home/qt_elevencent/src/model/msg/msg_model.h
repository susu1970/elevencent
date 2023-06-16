#ifndef MSG_MODEL_H_
#define MSG_MODEL_H_

#include<string>
#include"rsa.hpp"
#include"net_protocol.hpp"
#include<functional>
#include<QString>
#include"global.h"
#include<unordered_set>
#include<iomanip>
#include<algorithm>
#include<unordered_map>
#include<list>

namespace qt_elevencent{
  class MsgModel{
  public:
    class EventRespMsgUsers{
    public:
      std::string name="";
      EventRespMsgUsers(std::string name_):name(name_){}
    };
    class MsgUnit{
    public:
      resource_mask_t umask;
      resource_mask_t pmask;
      datetime_t update_time;
      std::string content="";
      MsgUnit(const MsgUnit&r){
	*this=r;
      }
      MsgUnit&operator=(const MsgUnit&r){
	umask=r.umask;
	pmask=r.pmask;
	update_time=r.update_time;
	content=r.content;
	return *this;
      }
    };
    std::unordered_set<std::string>msgUsers;
    std::unordered_map<std::string,std::list<MsgUnit>>userMsg;
    void insertMsg(std::string user,MsgUnit&msg){
      userMsg[user].push_back(msg);
      userMsg[user].sort([](MsgUnit&l,MsgUnit&r)->bool{
	return l.update_time>r.update_time;
      });
    }
    void clear(){
      msgUsers.clear();
      userMsg.clear();
    }
  };
}

#endif
