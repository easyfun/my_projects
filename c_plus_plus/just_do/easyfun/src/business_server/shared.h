#ifndef LLJZ_DISK_SHARED_H_
#define LLJZ_DISK_SHARED_H_

#include "tbsys.h"
#include "tbnet.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <stdio.h>
#include <string.h>

#include <openssl/evp.h>  
#include <openssl/bio.h>  
#include <openssl/buffer.h> 

#include <openssl/md5.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

using namespace tbnet;
using namespace tbsys;
using namespace rapidjson;

namespace lljz {
namespace disk {

    class BasePacket;
    class PacketFactory;
    class RequestPacket;
    class ResponsePacket;
    class IServerEntry;
    class ConnectionManagerToAccess;
    class ManagerClient;
    
}
}

#include "protocol.hpp"
#include "base_packet.hpp"
#include "packet_factory.hpp"
#include "request_packet.hpp"
#include "response_packet.hpp"
#include "iserver_entry.h"
#include "global.h"
#include "connection_manager_to_access.h"
#include "manager_client.h"
#include "redis_client.h"
#include "redis_client_manager.h"
#include "user_manager.h"
#include "shared.h"
#include "handler_router.h"
#include "public_echo_test_req_handler.h"
#include "handler.h"

namespace lljz {
namespace disk {


}
}

#endif