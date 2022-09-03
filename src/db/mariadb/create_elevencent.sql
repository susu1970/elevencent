drop database if exists `elevencent`;
create database `elevencent` default charset=utf8mb4;
use `elevencent`;

drop table if exists `user_resource`;
create table `user_resource`(
`user_resource_id` int unsigned not null,
`resource_id` int unsigned not null,
`resource_mask` bigint unsigned not null default 0,
`create_time` datetime default current_timestamp, 
`update_time` datetime default current_timestamp on update current_timestamp,
primary key(`user_resource_id`,`resource_id`)
)engine=InnoDB;

drop table if exists `resource`;
create table `resource`(
`resource_id` int unsigned not null primary key,
`resource_mask` bigint unsigned not null,
`resource_ref` int unsigned not null default 0
)engine=InnoDB;

drop table if exists `name_resource`;
create table `name_resource`(
`resource_id` int unsigned not null primary key,
`name` varchar(255) not null
)engine=InnoDB;

drop table if exists `file_resource`;
create table `file_resource`(
`resource_id` int unsigned not null primary key,
`file_name` varchar(1026) not null,
`file_location` varchar(10240) not null 
)engine=InnoDB;

drop table if exists `passwd_resource`;
create table `passwd_resource`(
`resource_id` int unsigned not null primary key,
`resource_mask` bigint unsigned not null default 0,
`passwd` varchar(4096) default null 
)engine=InnoDB;

drop table if exists `post_resource`;
create table `post_resource`(
`post_resource_id` int unsigned not null primary key,
`resource_id` int unsigned,	
`resource_mask` bigint unsigned not null default 0	
)engine=InnoDB;

drop table if exists `post_content_resource`;
create table `post_content_resource`(
`resource_id` int unsigned not null primary key,
`post_content` longblob default null 
)engine=InnoDB;
