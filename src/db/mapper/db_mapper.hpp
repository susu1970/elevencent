#ifndef _ELEVENCENT_DB_MAPPER_HPP_
#define _ELEVENCENT_DB_MAPPER_HPP_

#include<string>

namespace elevencent{
  typedef unsigned int resource_id_t;
  #define DB_MAX_RESOURCE_ID_NUM 4294967296
  #define DB_RESOURCE_ID_LOOP_INCRE(resourceId) do{if(++resourceId==DB_MAX_RESOURCE_ID_NUM)resourceId=0;}while(0)
  typedef unsigned long resource_mask_t;
  typedef unsigned long datetime_t;
  
  class DbMapper{
  public:
    class Resource;
    class UserResource;
    class NameResource;
    class FileResource;
    class PasswdResource;
    class PostResource;
    class PostContentResource;
    class Resource_Optimize{
    public:
      resource_mask_t m_resourceMask;
      unsigned int m_resourceRef;
      Resource_Optimize(){}
      Resource_Optimize(resource_mask_t resourceMask,unsigned int resourceRef):m_resourceMask(resourceMask),m_resourceRef(resourceRef){}
      Resource_Optimize(const Resource&resource):Resource_Optimize(resource.m_resourceMask,resource.m_resourceRef){}
      const Resource_Optimize&operator=(const Resource&resource){
	m_resourceMask=resource.m_resourceMask;
	m_resourceRef=resource.m_resourceRef;
	return *this;
      }
      bool operator==(const Resource&a){
	return m_resourceMask==a.m_resourceMask&&m_resourceRef==a.m_resourceRef;
      }
      bool operator==(const Resource_Optimize&a){
	return m_resourceMask==a.m_resourceMask&&m_resourceRef==a.m_resourceRef;
      }
      size_t size()const{
	return sizeof(m_resourceMask)+sizeof(m_resourceRef);
      }
    };
    class Resource:public Resource_Optimize{
    public:
      resource_id_t m_resourceId;
      Resource(){}
      Resource(resource_id_t resourceId,resource_mask_t resourceMask,unsigned int resourceRef):Resource_Optimize(resourceMask,resourceRef),m_resourceId(resourceId){}
      size_t size()const{
	return Resource_Optimize::size()+sizeof(m_resourceId);
      }
    };
    class UserResource_Optimize{
    public:
      resource_mask_t m_resourceMask;
      datetime_t m_createTime;
      datetime_t m_updateTime;
      UserResource_Optimize(){}
      UserResource_Optimize(resource_mask_t resourceMask,datetime_t createTime,datetime_t updateTime):m_resourceMask(resourceMask),m_createTime(createTime),m_updateTime(updateTime){}
      UserResource_Optimize(const UserResource&userResource):UserResource_Optimize(userResource.m_resourceMask,userResource.m_createTime,userResource.m_updateTime){}
      const UserResource_Optimize&operator=(const UserResource&userResource){
	m_resourceMask=userResource.m_resourceMask;
	m_createTime=userResource.m_createTime;
	m_updateTime=userResource.m_updateTime;
	return *this;
      }
      bool operator==(const UserResource_Optimize&a){
	return m_resourceMask==a.m_resourceMask&&m_createTime==a.m_createTime&&m_updateTime==a.m_updateTime;
      }
      bool operator==(const UserResource&a){
	return m_resourceMask==a.m_resourceMask&&m_createTime==a.m_createTime&&m_updateTime==a.m_updateTime;
      }
      size_t size()const{
	return sizeof(m_resourceMask)+sizeof(m_createTime)+sizeof(m_updateTime);
      }      
    };
    class UserResource:public UserResource_Optimize{
    public:
      resource_id_t m_userResourceId;
      resource_id_t m_resourceId;
      UserResource(){}
      UserResource(resource_id_t userResourceId,resource_id_t resourceId,resource_mask_t resourceMask,datetime_t createTime,datetime_t updateTime):UserResource_Optimize(resourceMask,createTime,updateTime),m_userResourceId(userResourceId),m_resourceId(resourceId){}
      size_t size()const{
	return UserResource_Optimize::size()+sizeof(m_userResourceId)+sizeof(m_resourceId);
      }
    };
    class NameResource_Optimize{
    public:
      std::string m_name="";//using non-initialized std::string will cause crash!
      NameResource_Optimize(){}
      NameResource_Optimize(std::string name):m_name(name){}
      NameResource_Optimize(const NameResource&nameResource):NameResource_Optimize(nameResource.m_name){}
      const NameResource_Optimize&operator=(const NameResource&nameResource){
	m_name=nameResource.m_name;
	return *this;
      }
      bool operator==(const NameResource_Optimize&a){
	return m_name==a.m_name;
      }
      bool operator==(const NameResource&a){
	return m_name==a.m_name;
      }      
      size_t size()const{
	return m_name.size()+sizeof(m_name);
      }
    };
    class NameResource:public NameResource_Optimize{
    public:
      resource_id_t m_resourceId;
      NameResource(){}
      NameResource(resource_id_t resourceId,std::string name):NameResource_Optimize(name),m_resourceId(resourceId){}
      size_t size()const{
	return NameResource_Optimize::size()+sizeof(m_resourceId);
      }
    };
    class FileResource_Optimize{
    public:
      std::string m_fileName="";//using non-initialized std::string will cause crash!
      std::string m_fileLocation="";//using non-initialized std::string will cause crash!
      FileResource_Optimize(){}
      FileResource_Optimize(std::string fileName,std::string fileLocation):m_fileName(fileName),m_fileLocation(fileLocation){}
      FileResource_Optimize(const FileResource&fileResource):FileResource_Optimize(fileResource.m_fileName,fileResource.m_fileLocation){}
      const FileResource_Optimize&operator=(const FileResource&fileResource){
	m_fileName=fileResource.m_fileName;
	m_fileLocation=fileResource.m_fileLocation;
	return *this;
      }
      bool operator==(const FileResource_Optimize&a){
	return m_fileName==a.m_fileName&&m_fileLocation==a.m_fileLocation;
      }
      bool operator==(const FileResource&a){
	return m_fileName==a.m_fileName&&m_fileLocation==a.m_fileLocation;
      }      
      size_t size()const{
	return sizeof(m_fileLocation)+sizeof(m_fileLocation)+m_fileLocation.size()+m_fileLocation.size();
      }
    };
    class FileResource:public FileResource_Optimize{
    public:
      resource_id_t m_resourceId;
      FileResource(){}
      FileResource(resource_id_t resourceId,std::string fileName,std::string fileLocation):FileResource_Optimize(fileName,fileLocation),m_resourceId(resourceId){}
      size_t size()const{
	return FileResource_Optimize::size()+sizeof(m_resourceId);
      }
    };
    class PasswdResource_Optimize{
    public:
      resource_mask_t m_resourceMask;
      std::string m_passwd="";//using non-initialized std::string will cause crash!
      PasswdResource_Optimize(){}
      PasswdResource_Optimize(resource_mask_t resourceMask,std::string passwd):m_resourceMask(resourceMask),m_passwd(passwd,passwd.size()){}
      PasswdResource_Optimize(const PasswdResource&passwdResource):PasswdResource_Optimize(passwdResource.m_resourceMask,passwdResource.m_passwd){}
      const PasswdResource_Optimize&operator=(const PasswdResource&passwdResource){
	m_resourceMask=passwdResource.m_resourceMask;
	m_passwd=passwdResource.m_passwd;
	return *this;
      }
      bool operator==(const PasswdResource_Optimize&a){
	return m_resourceMask==a.m_resourceMask&&m_passwd==a.m_passwd;
      }
      bool operator==(const PasswdResource&a){
	return m_resourceMask==a.m_resourceMask&&m_passwd==a.m_passwd;
      }
      size_t size()const{
	return sizeof(m_resourceMask)+m_passwd.size()+sizeof(m_passwd);
      }
    };
    class PasswdResource:public PasswdResource_Optimize{
    public:
      resource_id_t m_resourceId;
      PasswdResource(){}
      PasswdResource(resource_id_t resourceId,resource_mask_t resourceMask,std::string passwd):PasswdResource_Optimize(resourceMask,passwd),m_resourceId(resourceId){}
      size_t size()const{
	return PasswdResource_Optimize::size()+sizeof(m_resourceId);
      }
    };
    class PostResource_Optimize{
    public:
      resource_id_t m_resourceId;
      resource_mask_t m_resourceMask;
      PostResource_Optimize(){}
      PostResource_Optimize(resource_id_t resourceId,resource_mask_t resourceMask):m_resourceId(resourceId),m_resourceMask(resourceMask){}
      PostResource_Optimize(const PostResource&postResource):PostResource_Optimize(postResource.m_resourceId,postResource.m_resourceMask){}
      const PostResource_Optimize&operator=(const PostResource&postResource){
	m_resourceId=postResource.m_resourceId;
	m_resourceMask=postResource.m_resourceMask;
	return *this;
      }
      bool operator==(const PostResource_Optimize&a){
	return m_resourceId==a.m_resourceId&&m_resourceMask==a.m_resourceMask;
      }
      bool operator==(const PostResource&a){
	return m_resourceId==a.m_resourceId&&m_resourceMask==a.m_resourceMask;
      }
      size_t size()const{
	return sizeof(m_resourceId)+sizeof(m_resourceMask);
      }
    };
    class PostResource:public PostResource_Optimize{
    public:
      resource_id_t m_postResourceId;
      PostResource(){}
      PostResource(resource_id_t postResourceId,resource_id_t resourceId,resource_mask_t resourceMask):PostResource_Optimize(resourceId,resourceMask),m_postResourceId(postResourceId){}
      size_t size()const{
	return PostResource_Optimize::size()+sizeof(m_postResourceId);
      }
    };
    class PostContentResource_Optimize{
    public:
      std::string m_postContent="";//using non-initialized std::string will cause crash!
      PostContentResource_Optimize(){}
      PostContentResource_Optimize(std::string postContent):m_postContent(postContent){}
      PostContentResource_Optimize(const PostContentResource&postContentResource):PostContentResource_Optimize(postContentResource.m_postContent){}
      const PostContentResource_Optimize&operator=(const PostContentResource&postContentResource){
	m_postContent=postContentResource.m_postContent;
	return*this;
      }
      bool operator==(const PostContentResource_Optimize&a){
	return m_postContent==a.m_postContent;
      }
      bool operator==(const PostContentResource&a){
	return m_postContent==a.m_postContent;
      }
      size_t size()const{
	return m_postContent.size()+sizeof(m_postContent);
      }
    };
    class PostContentResource:public PostContentResource_Optimize{
    public:
      resource_id_t m_resourceId;
      PostContentResource(){}
      PostContentResource(resource_id_t resourceId,std::string postContent):PostContentResource_Optimize(postContent),m_resourceId(resourceId){}
      size_t size()const{
	return PostContentResource_Optimize::size()+sizeof(m_resourceId);
      }
    };
  };
}

#endif
