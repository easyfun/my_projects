#ifndef LLJZ_DISK_MANAGER_CLIENT_H
#define LLJZ_DISK_MANAGER_CLIENT_H

#include "tbsys.h"
#include "tbnet.h"
#include "packet_factory.hpp"

namespace lljz {
namespace disk {

class ManagerClient : public tbnet::IServerAdapter,
public tbnet::IPacketQueueHandler {
public:
    ManagerClient();
    ~ManagerClient();

    void Start();
    void Stop();

    //IServerAdapter interface
    virtual tbnet::IPacketHandler::HPRetCode 
    handlePacket(tbnet::Connection *connection, tbnet::Packet *packet);
    //接收客户端packet处理线程

    // IPacketQueueHandler interface
    bool handlePacketQueue(tbnet::Packet * apacket, void *args);

    //IBusinessPacketHandler interface
    bool BusinessHandlePacket(
        tbnet::Packet *packet, void *args);

private:
    inline int Initialize();
    inline int Destroy();

private:
    PacketFactory packet_factory_;
    tbnet::DefaultPacketStreamer packet_streamer_;
    tbnet::Transport transport_for_client_;

    tbnet::PacketQueueThread task_queue_thread_;
    //客户端packet统计信息
    uint64_t client_disconn_throw_packets_;   // 客户端连接失效丢弃的请求包数
    uint64_t queue_thread_timeout_throw_packets_;   // PacketQueueThread排队超时丢弃的请求包数

    //uint16_t self_server_type_;
    uint64_t self_server_id_;
    char self_server_spec_[100];
};

}
}

#endif
