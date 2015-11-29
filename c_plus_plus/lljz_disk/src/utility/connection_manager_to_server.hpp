#ifndef LLJZ_DISK_CONNECTION_MANAGER_TO_SERVER_H
#define LLJZ_DISK_CONNECTION_MANAGER_TO_SERVER_H

#include <ext/hash_map>
#include "tbsys.h"
#include "tbnet.h"
#include "load_connection_manager.hpp"

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

class ConnectionManagerToServer:public tbsys::Runnable {
public:
    ConnectionManagerToServer(tbnet::Transport* transport,
        tbnet::IPacketStreamer* packet_streamer,
        tbnet::IPacketHandler* packet_handler);
    ~ConnectionManagerToServer();

    bool start(const char* config_server_spec);
    bool stop();
    bool wait();
    void destroy();
    //Runnable
    void run(tbsys::CThread* thread, void* arg);


    //Post packet to server
    bool PostPacket(tbnet::Packet* packet, void* arg=NULL);

public:

private:
    tbnet::Transport* transport_;
    tbnet::IPacketStreamer* packet_streamer_;
    tbnet::IPacketHandler* packet_handler_;

    bool stop_;
    tbsys::CThread get_server_url_thread_;
    char config_server_spec_[100];
    tbnet::Connection* conn_to_config_server_;

    //not include config_server,access_server
    std::vector<uint32_t> depend_server_type_;
    std::vector<ServerURL*> server_url_;

    //server_type---LoadConnectionManager*
    __gnu_cxx::hash_map<uint32_t,LoadConnectionManager*> conn_manager_;
    tbsys::CThreadMutex mutex_;
};


}
}

#endif