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
#ifndef TAIR_PACKETS_BASE_H
#define TAIR_PACKETS_BASE_H
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

//config_server-获取服务列表
#define CONFIG_SERVER_GET_SERVICE_LIST_REQ   1
#define CONFIG_SERVER_GET_SERVICE_LIST_RESP  2

   enum {
      DIRECTION_RECEIVE = 1,
      DIRECTION_SEND
   };

   enum {
      TAIR_STAT_TOTAL = 1,
      TAIR_STAT_SLAB = 2,
      TAIR_STAT_HASH = 3,
      TAIR_STAT_AREA = 4,
      TAIR_STAT_GET_MAXAREA = 5,
      TAIR_STAT_ONEHOST = 256
   };

   class BasePacket : public tbnet::Packet {
   public:
      BasePacket()
      {
         connection = NULL;
         direction = DIRECTION_SEND;
         no_free = false;
         server_flag = 0;
         request_time = 0;
         fixed_size = 0;
      }

      virtual size_t size()
      {
        return 0;
      }

      virtual uint16_t ns()
      {
        return 0;
      }

      virtual ~BasePacket()
      {
      }

      // Connection
      tbnet::Connection *get_connection()
      {
         return connection;
      }

      // connection
      void set_connection(tbnet::Connection *connection)
      {
         this->connection = connection;
      }

      // direction
      void set_direction(int direction)
      {
         this->direction = direction;
      }

      // direction
      int get_direction()
      {
         return direction;
      }

      void free()
      {
         if (!no_free) {
            delete this;
         }
      }

      void set_no_free()
      {
         no_free = true;
      }

   private:
      BasePacket& operator = (const BasePacket&);

      tbnet::Connection *connection;
      int direction;
      bool no_free;
   public:
      uint8_t server_flag;
      int64_t request_time;
   protected:
      size_t fixed_size;
   };

}
}
#endif
