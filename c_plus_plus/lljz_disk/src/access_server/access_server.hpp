#ifndef LLJZ_DISK_ACCESS_SERVER_H_
#define LLJZ_DISK_ACCESS_SERVER_H_

#include "tbsys.h"
#include "tbnet.h"
#include "packet_factory.hpp"

namespace lljz {
namespace disk {

class AccessServer : public tbnet::IServerAdapter,
    public tbnet::IPacketQueueHandler {
public:
    AccessServer();
    ~AccessServer();

    void Start();
    void Stop();

    //IServerAdapter interface
    virtual tbnet::IPacketHandler::HPRetCode handlePacket(tbnet::Connection *connection, tbnet::Packet *packet);

    // IPacketQueueHandler interface
    bool handlePacketQueue(tbnet::Packet * apacket, void *args);

private:
    inline int Initialize();
    inline int Destroy();

private:
    PacketFactory packet_factory_;
    tbnet::DefaultPacketStreamer packet_streamer_;
    tbnet::Transport packet_transport_;
    tbnet::Transport to_server_transport_;//send packet to business server

    tbnet::PacketQueueThread task_queue_thread_;
//    server_conf_thread my_server_conf_thread_;
    //客户端packet统计信息
    uint64_t clientDisconnThrowPackets_;   // 客户端连接失效丢弃的请求包数
    uint64_t queueThreadTimeoutThrowPackets_;   // PacketQueueThread排队超时丢弃的请求包数
};

}
}

#endif
