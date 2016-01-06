#ifndef LLJZ_DISK_MANAGER_SERVER_H
#define LLJZ_DISK_MANAGER_SERVER_H

/*
#include <ext/hash_map>
#include "tbsys.h"
#include "tbnet.h"
#include "load_connection.h"
*/

namespace lljz {
namespace disk {

typedef __gnu_cxx::hash_map<uint16_t,LoadConnection*> ConnectionMap;

//class ManagerClient;

// public tbsys::Runnable,
class ManagerServer:public tbnet::IPacketHandler,
public tbnet::IPacketQueueHandler {
public:
    ManagerServer();
    ~ManagerServer();

    //IPacketHandler interface
    tbnet::IPacketHandler::HPRetCode 
        handlePacket(tbnet::Packet *packet, void *args);

    // IPacketQueueHandler interface
    bool handlePacketQueue(tbnet::Packet * apacket, void *args);

    bool start();
    bool stop();
    bool wait();
    void destroy();

    //Runnable
    //void run(tbsys::CThread* thread, void* arg);

    //Post packet to server
    bool postPacket(uint16_t server_type,
        tbnet::Packet* packet, 
        void* args=NULL);

    void set_manager_client(ManagerClient* manager_client) {
        manager_client_=manager_client;
    }

    void set_channel_pool(tbnet::ChannelPool* channel_pool) {
        channel_pool_=channel_pool;
    }

private:
    ManagerClient* manager_client_;
    tbnet::ChannelPool* channel_pool_;

    PacketFactory packet_factory_;
    tbnet::DefaultPacketStreamer packet_streamer_;
    tbnet::Transport transport_for_server_;

    //tbnet::PacketQueueThread task_queue_thread_;

    bool stop_;
    //tbsys::CThread timer_thread_;

    //当前服务器服务配置
    uint16_t self_server_type_;
    uint64_t self_server_id_;
    char self_server_spec_[100];

    //依赖后端业务服务
    ConnectionMap conn_map_;
};


}
}

#endif