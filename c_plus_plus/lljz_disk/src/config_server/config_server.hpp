#ifndef LLJZ_DISK_CONFIG_SERVER_H_
#define LLJZ_DISK_CONFIG_SERVER_H_

#include "tbsys.h"
#include "tbnet.h"
#include "packet_factory.hpp"

namespace lljz {
namespace disk {

class ConfigServer : public tbnet::IServerAdapter,
    public tbnet::IPacketQueueHandler {
public:
    ConfigServer();
    ~ConfigServer();

    void Start();
    void Stop();

    //IServerAdapter interface
    virtual tbnet::IPacketHandler::HPRetCode handlePacket(tbnet::Connection *connection, tbnet::Packet *packet);

    // IPacketQueueHandler interface
    bool handlePacketQueue(tbnet::Packet * apacket, void *args);

private:
    PacketFactory packet_factory_;
    tbnet::DefaultPacketStreamer packet_streamer_;
    tbnet::Transport packet_transport_;
//    tbnet::Transport heartbeat_transport_;

    tbnet::PacketQueueThread task_queue_thread_;
//    server_conf_thread my_server_conf_thread_;

private:
    inline int Initialize();
    inline int Destroy();
};

}
}

#endif
