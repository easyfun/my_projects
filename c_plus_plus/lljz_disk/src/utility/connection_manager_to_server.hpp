#ifndef LLJZ_DISK_CONNECTION_MANAGER_TO_SERVER_H
#define LLJZ_DISK_CONNECTION_MANAGER_TO_SERVER_H

#include <ext/hash_map>
#include "tbsys.h"
#include "tbnet.h"
#include "load_connection_manager.hpp"
#include "packet_handler_to_server.hpp"

namespace lljz {
namespace disk {

struct ServerURL {
    char spec_[100];
    uint32_t server_type_;
    uint32_t server_id_;
    int changed_;//0-not,1-add,2-remove

    ServerURL() {
        memset(this,0,sizeof(ServerURL));
    }
};

class ConnectionManagerToServer:public tbsys::Runnable,
public tbnet::IPacketHandler {
public:
    ConnectionManagerToServer(tbnet::Transport* transport,
        tbnet::IPacketStreamer* packet_streamer,
        IBusinessPacketHandler* bph);
    ~ConnectionManagerToServer();

    //chat with config_server
    //IPacketHandler interface
    //discard disconnect with config_server
    //autoly reconnect to config_server
    tbnet::HPRetCode handlePacket(tbnet::Packet *packet, 
        void *args);
    bool start();
    bool stop();
    bool wait();
    void destroy();

    //Runnable
    void run(tbsys::CThread* thread, void* arg);
    void CheckConfigServer();
    void CheckReconnServer();

    //Post packet to server
    bool PostPacket(uint16_t server_type,
        tbnet::Packet* packet, 
        void* args=NULL);

/*
    //server_id断开自动重连
    void DisToReConnect(uint64_t server_id);
*/
    //从配置服务器获取服务列表
    bool GetServiceListResp(tbnet::Packet * apacket, void *args);

private:
    tbnet::Transport* transport_;
    tbnet::IPacketStreamer* packet_streamer_;

    PacketHandlerToServer packet_handler_to_server_;

    //当前服务器服务配置
    uint16_t self_server_type_;
    uint64_t self_server_id_;
    char self_server_spec_[100];

    //配置服务器
    char config_server_spec_[100];
    bool stop_;
    tbsys::CThread timer_thread_;
    tbnet::Connection* conn_to_config_server_;

    //依赖后端业务服务
    //not include config_server,access_server
    std::vector<uint16_t> depend_server_type_;
    std::vector<ServerURL*> server_url_;

    //server_type---LoadConnectionManager*
    __gnu_cxx::hash_map<uint16_t,
        LoadConnectionManager*> conn_manager_;
//    tbsys::CThreadMutex mutex_;
    tbsys::CRWSimpleLock conn_manager_rw_lock_;

};


}
}

#endif