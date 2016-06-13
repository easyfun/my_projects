#ifndef LLJZ_DISK_SHARED_H_
#define LLJZ_DISK_SHARED_H_

#include "tbsys.h"
#include "tbnet.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace lljz {
namespace disk {

    class BasePacket;
    class PacketFactory;
    class RequestPacket;
    class ResponsePacket;
    class IServerEntry;
    class ManagerClient;
    class ManagerServer;
    class LoadConnection;
    
}
}

#include "global.h"
#include "protocol.hpp"
#include "base_packet.hpp"
#include "packet_factory.hpp"
#include "request_packet.hpp"
#include "response_packet.hpp"
#include "iserver_entry.h"
#include "manager_client.h"
#include "manager_server.h"
#include "load_connection.h"

namespace lljz {
namespace disk {

typedef struct RequestInfo {
    uint32_t chid_;
    uint16_t src_type_;
    uint64_t src_id_;
    uint64_t append_args_;
    tbnet::Connection* conn_;
    int64_t expire_time_;
}RequestInfo;

}
}

#endif