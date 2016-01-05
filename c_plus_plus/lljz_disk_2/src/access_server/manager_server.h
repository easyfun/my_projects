#ifndef LLJZ_DISK_MANAGER_SERVER_H
#define LLJZ_DISK_MANAGER_SERVER_H

#include <ext/hash_map>
#include "tbsys.h"
#include "tbnet.h"

namespace lljz {
namespace disk {

class ManagerServer:public tbsys::Runnable,
public tbnet::IPacketHandler,
public tbnet::IPacketQueueHandler {
public:
    ManagerServer();
    ~ManagerServer();

    //IPacketHandler interface
    tbnet::IPacketHandler::HPRetCode 
        handlePacket(tbnet::Packet *packet, void *args);

    // IPacketQueueHandler interface
    //bool handlePacketQueue(tbnet::Packet * apacket, void *args);

    bool start();
    bool stop();
    bool wait();
    void destroy();

    //Runnable
    //void run(tbsys::CThread* thread, void* arg);

    //Post packet to server
    bool PostPacket(uint16_t server_type,
        tbnet::Packet* packet, 
        void* args=NULL);

private:
    PacketFactory packet_factory_;
    tbnet::DefaultPacketStreamer packet_streamer_;
    tbnet::Transport transport_for_server_;

    tbnet::PacketQueueThread task_queue_thread_;

    bool stop_;
    //tbsys::CThread timer_thread_;

    //当前服务器服务配置
    uint16_t self_server_type_;
    uint64_t self_server_id_;
    char self_server_spec_[100];

    //依赖后端业务服务
};


}
}

#endif