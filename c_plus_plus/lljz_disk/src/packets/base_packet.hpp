/*
 * (C) 2007-2010 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * packet code and base packet are defined here
 *
 * Version: $Id: base_packet.hpp 998 2012-07-20 05:45:55Z zongdai $
 *
 * Authors:
 *   ruohai <ruohai@taobao.com>
 *     - initial release
 *
 */
#ifndef LLJZ_DISK_PACKETS_BASE_H
#define LLJZ_DISK_PACKETS_BASE_H
#include <string>
#include <map>
#include <set>
#include <vector>
#include <tbsys.h>
#include <tbnet.h>
#include <stdint.h>
#include <zlib.h>

using namespace std;

namespace lljz {
namespace disk {

#define PACKET_IN_PACKET_QUEUE_THREAD_MAX_TIME  180000000   //180s
#define PACKET_WAIT_FOR_SERVER_MAX_TIME         180000000   //180s

//Packet type
enum {
   REQUEST_PACKET=0,
   RESPONSE_PACKET=1
};

//uint64_t [1,65535]
//server type
enum {
   SERVER_TYPE_CONFIG_SERVER=1,   //配置服务
   SERVER_TYPE_ACCESS_SERVER=2,   //接入服务
   SERVER_TYPE_ACCOUNT_SERVER=3,  //账号服务
   SERVER_TYPE_FILE_SERVER=4,    //文件服务器

   SERVER_TYPE_CLIENT_LINUX=65000,  //linux客户端
   SERVER_TYPE_PYTHON_CLIENT_LINUX=65001  //linux python客户端
};

//public
enum {
   //回显调试
   PUBLIC_ECHO_TEST_REQ=0,
   PUBLIC_ECHO_TEST_RESP=1,

   //注册
   PUBLIC_REGISTER_REQ=2,
   PUBLIC_REGISTER_RESP=3
};

//config_server
enum ConfigServerMsg{
   //获取服务列表
   CONFIG_SERVER_GET_SERVICE_LIST_REQ=1000,
   CONFIG_SERVER_GET_SERVICE_LIST_RESP=1001
};

//account_server
enum AccountServerMsg {
   ACCOUNT_SERVER_REGISTER_ACCOUNT_REQ=1000,//注册账号
   ACCOUNT_SERVER_REGISTER_ACCOUNT_RESP=1001,

   ACCOUNT_SERVER_SET_ACCOUNT_INFO_REQ=1002,//设置账号信息
   ACCOUNT_SERVER_SET_ACCOUNT_INFO_RESP=1003,

   ACCOUNT_SERVER_MODIFY_LOGIN_PASSWORD_REQ=1004,//修改登陆密码
   ACCOUNT_SERVER_MODIFY_LOGIN_PASSWORD_RESP=1005,

   ACCOUNT_SERVER_LOGIN_REQ=1006,//登陆
   ACCOUNT_SERVER_LOGIN_RESP=1007,

   ACCOUNT_SERVER_LOGOUT_REQ=1008,//登出
   ACCOUNT_SERVER_LOGOUT_RESP=1009
};

//file_server
enum FileServerMsg {
   //新建文件夹
   FILE_SERVER_CREATE_FOLDER_REQ=1000,
   FILE_SERVER_CREATE_FOLDER_RESP=1001,

   //修改文件/文件夹属性
   FILE_SERVER_MODIFY_PROPERTY_REQ=1002,
   FILE_SERVER_MODIFY_PROPERTY_RESP=1003,

   //上传文件
   FILE_SERVER_UPLOAD_FILE_REQ=1004,
   FILE_SERVER_UPLOAD_FILE_RESP=1005,

   //下载文件
   FILE_SERVER_DOWNLOAD_FILE_REQ=1006,
   FILE_SERVER_DOWNLOAD_FILE_RESP=1007,

   //下载文件
   FILE_SERVER_DOWNLOAD_FILE_GET_INFO_REQ=1008,
   FILE_SERVER_DOWNLOAD_FILE_GET_INFO_RESP=1009,

   //删除文件/文件夹
   FILE_SERVER_DELETE_FILE_OR_FOLDER_REQ=1010,
   FILE_SERVER_DELETE_FILE_OR_FOLDER_RESP=1011,

   //查找文件
   FILE_SERVER_SEARCH_FILE_REQ=1012,
   FILE_SERVER_SEARCH_FILE_RESP=1013
};


enum {
   DIRECTION_RECEIVE = 1,
   DIRECTION_SEND
};

   class BasePacket : public tbnet::Packet {
   public:
      BasePacket() {
         connection_ = NULL;
         args_=NULL;
         direction_ = DIRECTION_SEND;
         no_free_=false;
         recv_time_ = 0;
      }

      virtual ~BasePacket() {
      }

      // Connection
      tbnet::Connection* get_connection() {
         return connection_;
      }

      // connection
      void set_connection(tbnet::Connection *connection) {
         connection_ = connection;
      }

      void* get_args() {
         return args_;
      }

      void set_args(void* args) {
         args_=args;
      }

      // direction
      void set_direction(int direction) {
         direction_ = direction;
      }

      // direction
      int get_direction() {
         return direction_;
      }

      // recv_time
      void set_recv_time(uint64_t recv_time) {
         recv_time_=recv_time;
      }

      uint64_t get_recv_time() {
         return recv_time_;
      }

      void free() {
//         TBSYS_LOG(TRACE,"BasePacket::free:addr=%u,no_free_=%d",this,no_free_);
         if (!no_free_) {
            delete this;
         }
      }

      void set_no_free() {
         no_free_ = true;
      }

   private:
      BasePacket& operator = (const BasePacket&);

      tbnet::Connection *connection_;
      int direction_;
      void* args_;//作为客户端，handlePacket(,args)
      bool no_free_;

   protected:
      int64_t recv_time_;
   };

}
}
#endif
