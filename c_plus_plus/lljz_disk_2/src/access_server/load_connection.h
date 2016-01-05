#ifndef LLJZ_DISK_LOAD_CONNECTION_H
#define LLJZ_DISK_LOAD_CONNECTION_H

#include "tbnet.h"
#include "tbsys.h"

namespace lljz {
namespace disk {

#define MAX_SERVER_NUM 16

class LoadConnection {
public:
    LoadConnection(uint16_t server_type);
    ~LoadConnection();

    bool Connect(tbnet::Transport* transport);
//    void DisConnect();

//    bool postPacket();

private:
    uint16_t server_type_;
    tbnet::Connection* conns_[MAX_SERVER_NUM];
    int conn_num_;
};

}
}

#endif