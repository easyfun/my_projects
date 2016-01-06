#ifndef LLJZ_DISK_MANAGER_CLIENT_H
#define LLJZ_DISK_MANAGER_CLIENT_H

//#include "tbsys.h"
//#include "tbnet.h"
//#include "packet_factory.hpp"

namespace lljz {
namespace disk {

//class ManagerServer;

class ManagerClient:public tbsys::Runnable,
public tbnet::IServerAdapter,
public tbnet::IPacketQueueHandler {
public:
    ManagerClient();
    ~ManagerClient();

    bool start();
    bool wait();
    bool stop();
    int initialize();
    void destroy();

    //IServerAdapter interface
    virtual tbnet::IPacketHandler::HPRetCode 
    handlePacket(tbnet::Connection *connection, tbnet::Packet *packet);
    //接收客户端packet处理线程

    // IPacketQueueHandler interface
    bool handlePacketQueue(tbnet::Packet * apacket, void *args);

    //Runnable
    void run(tbsys::CThread* thread, void* arg);

    void set_manager_server(ManagerServer* manager_server) {
        manager_server_=manager_server;
    }

    void set_channel_pool(tbnet::ChannelPool* channel_pool) {
        channel_pool_=channel_pool;
    }


private:
    ManagerServer* manager_server_;
    tbnet::ChannelPool* channel_pool_;

    PacketFactory packet_factory_;
    tbnet::DefaultPacketStreamer packet_streamer_;
    tbnet::Transport transport_for_client_;

    tbnet::PacketQueueThread task_queue_thread_;

    bool stop_;
    tbsys::CThread timer_thread_;

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
