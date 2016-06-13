#ifndef LLJZ_DISK_ACCOUNT_MANAGER_CLIENT_H_
#define LLJZ_DISK_ACCOUNT_MANAGER_CLIENT_H_

namespace lljz {
namespace disk {

class ManagerClient : public tbnet::IServerAdapter,
public tbnet::IPacketQueueHandler {
public:
    ManagerClient();
    ~ManagerClient();

    bool start();
    bool wait();
    bool stop();
    int initialize();
    int destroy();

    //IServerAdapter interface
    virtual tbnet::IPacketHandler::HPRetCode 
    handlePacket(tbnet::Connection *connection, tbnet::Packet *packet);
    //接收客户端packet处理线程

    // IPacketQueueHandler interface
    bool handlePacketQueue(tbnet::Packet * apacket, void *args);


private:
    PacketFactory packet_factory_;
    tbnet::DefaultPacketStreamer packet_streamer_;
    tbnet::Transport transport_for_client_;

    tbnet::PacketQueueThread task_queue_thread_;

    ConnectionManagerToAccess conn_manager_to_access_;
    
    //客户端packet统计信息
    uint64_t clientDisconnThrowPackets_;   // 客户端连接失效丢弃的请求包数
    uint64_t queueThreadTimeoutThrowPackets_;   // PacketQueueThread排队超时丢弃的请求包数

};

}
}

#endif
