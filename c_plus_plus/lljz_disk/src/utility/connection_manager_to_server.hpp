#ifndef LLJZ_DISK_CONNECTION_MANAGER_TO_SERVER_H
#define LLJZ_DISK_CONNECTION_MANAGER_TO_SERVER_H

#include <ext/hash_map>
#include "tbsys.h"
#include "tbnet.h"

namespace lljz {
namespace disk {

class ConnectionManagerToServer {
public:
    ConnectionManagerToServer();
    ~ConnectionManagerToServer();

    //注册来自客户端的连接
    //用于推送
    bool RegisterConnection(tbnet::Connection* connection);
    
    // 返回值无用
    bool PostPacket(,tbnet::Packet* packet, void* args=NULL);

public:

};


}
}

#endif