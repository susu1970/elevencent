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
#include<set>
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
      MsgUnit(){}
      MsgUnit(resource_mask_t umask_,resource_mask_t pmask_,datetime_t update_time_,std::string content_):umask(umask_),pmask(pmask_),update_time(update_time_),content(content_){}
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
      class Comp{
      public:
	bool operator()(const MsgUnit&l,const MsgUnit&r)const{
	  return l.update_time<r.update_time;
	}
      };
      bool operator==(const MsgUnit&r){
	return r.umask==umask&&r.pmask==pmask&&r.update_time==update_time&&r.content==content;
      }
      friend std::ostream&operator<<(std::ostream&os,const MsgUnit&m);
    };
    class EventRespUserReqMsg{
    public:
      std::string name="";
      MsgUnit msgUnit;
      std::string content="";
      resource_mask_t umask,pmask;
      datetime_t update_time;
      EventRespUserReqMsg(std::string name_,MsgUnit m):name(name_),msgUnit(m){}
    };    
    std::unordered_set<std::string>msgUsers;
    std::unordered_map<std::string,std::set<MsgUnit,MsgUnit::Comp>>userMsg;
    void insertMsg(std::string user,MsgUnit&msg){
      userMsg[user].insert(msg);
    }
    void clear(){
      msgUsers.clear();
      userMsg.clear();
    }
  };
  inline std::ostream&operator<<(std::ostream&os,const MsgModel::MsgUnit&m){
    os<<"umask: "<<m.umask<<", pmask: "<<m.pmask<<", update_time: "<<m.update_time<<", content: "<<m.content<<std::endl;
    return os;
  }
}

#endif
