#ifndef LLJZ_DISK_LOAD_CONNECTION_H
#define LLJZ_DISK_LOAD_CONNECTION_H

#include "tbnet.h"
#include "tbsys.h"

namespace lljz {
namespace disk {

#define MAX_SERVER_NUM 16

class LoadConnection {
public:
    LoadConnection(uint16_t server_type,
        tbnet::Transport* transport, 
        tbnet::IPacketStreamer* streamer, 
        tbnet::IPacketHandler* packetHandler);
    ~LoadConnection();

    void setDefaultQueueLimit(int queueLimit) {
        _queueLimit=queueLimit;
    }

    void setDefaultQueueTimeout(int queueTimeout) {
        _queueTimeout=queueTimeout;
    }

    bool Connect();
    void DisConnect();

    bool postPacket(tbnet::Packet *packet, 
        tbnet::IPacketHandler *packetHandler = NULL, 
        void *args = NULL, 
        bool noblocking = true);

private:
    uint16_t server_type_;
    tbnet::Transport *_transport;
    tbnet::IPacketStreamer *_streamer;
    tbnet::IPacketHandler *_packetHandler;
    int _queueLimit;
    int _queueTimeout;

    tbnet::Connection* conns_[MAX_SERVER_NUM];
    int conn_num_;
//    int last_send_conn_index_;

};

}
}

#endif