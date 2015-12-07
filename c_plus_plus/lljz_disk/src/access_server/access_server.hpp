#ifndef LLJZ_DISK_ACCESS_SERVER_H_
#define LLJZ_DISK_ACCESS_SERVER_H_

#include "tbsys.h"
#include "tbnet.h"
#include "packet_factory.hpp"
#include "connection_manager_to_server.hpp"
#include "ibusiness_packet_handler.h"

namespace lljz {
namespace disk {

class AccessServer : public tbnet::IServerAdapter,
public tbnet::IPacketQueueHandler,
public IBusinessPacketHandler {
public:
    AccessServer();
    ~AccessServer();

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
    tbnet::Transport from_client_transport_;
    tbnet::Transport to_server_transport_;//send packet to business server

    tbnet::PacketQueueThread task_queue_thread_;
//    server_conf_thread my_server_conf_thread_;
    //客户端packet统计信息
    uint64_t clientDisconnThrowPackets_;   // 客户端连接失效丢弃的请求包数
    uint64_t queueThreadTimeoutThrowPackets_;   // PacketQueueThread排队超时丢弃的请求包数

    ConnectionManagerToServer* conn_manager_to_srv_;
};

}
}

#endif
