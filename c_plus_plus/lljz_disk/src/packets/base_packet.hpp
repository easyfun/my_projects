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

#define PACKET_IN_PACKET_QUEUE_THREAD_MAX_TIME 180000000
#define PACKET_WAIT_FOR_SERVER_MAX_TIME 180000000

//Packet type
#define REQUEST_PACKET 0
#define RESPONSE_PACKET 1

//server type
enum {
   SERVER_TYPE_CONFIG_SERVER=0x0001,   //配置服务
   SERVER_TYPE_ACCESS_SERVER=0x0002    //接入服务
};

//config_server-获取服务列表
enum {
   CONFIG_SERVER_GET_SERVICE_LIST_REQ=0x0001,
   CONFIG_SERVER_GET_SERVICE_LIST_RESP=0x0002
};

enum {
   DIRECTION_RECEIVE = 1,
   DIRECTION_SEND
};

   class BasePacket : public tbnet::Packet {
   public:
      BasePacket() {
         connection_ = NULL;
         direction_ = DIRECTION_SEND;
         recv_time_ = 0;
      }

      virtual ~BasePacket() {
      }

      // Connection
      tbnet::Connection* get_gonnection() {
         return connection_;
      }

      // connection
      void set_connection(tbnet::Connection *connection) {
         connection_ = connection;
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

   private:
      BasePacket& operator = (const BasePacket&);

      tbnet::Connection *connection_;
      int direction_;

   protected:
      int64_t recv_time_;
   };

}
}
#endif
