#ifndef LLJZ_DISK_CONNECTION_MANAGER_TO_SERVER_H
#define LLJZ_DISK_CONNECTION_MANAGER_TO_SERVER_H

#include <ext/hash_map>
#include "tbsys.h"
#include "tbnet.h"
#include "load_connection_manager.hpp"

namespace lljz {
namespace disk {

/**********************************************************************/
// class PacketHandlerConnToServer
/**********************************************************************/
class ConnectionManagerToServer;
//only proc disconn_cmd when disconnect with business_server
class PacketHandlerConnToServer:public tbnet::IPacketHandler {
public:
    PacketHandlerConnToServer(ConnectionManagerToServer* cmts) {
        cmts_=cmts;
    }
    //IPacketHandler interface
    tbnet::HPRetCode handlePacket(Packet *packet, void *args);
private:
    ConnectionManagerToServer* cmts_;
};


/**********************************************************************/
// class PacketHandlerConnToCfgSrv
/**********************************************************************/
//only proc disconn_cmd when disconnect with cfg_srv
class PacketHandlerConnToCfgSrv:public tbnet::IPacketHandler {
public:
    //IPacketHandler interface
    tbnet::HPRetCode handlePacket(Packet *packet, void *args){

    }

};


/**********************************************************************/
// class ConnectionManagerToServer
/**********************************************************************/

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
public tbnet::IPacketQueueHandler,
public tbnet::IPacketHandler {
public:
    ConnectionManagerToServer(tbnet::Transport* transport,
        tbnet::IPacketStreamer* packet_streamer,
        tbnet::IPacketHandler* packet_handler);
    ~ConnectionManagerToServer();

    /* chat with config_server
    */
    //IPacketHandler interface
    tbnet::HPRetCode handlePacket(Packet *packet, void *args);
    // IPacketQueueHandler interface
    bool handlePacketQueue(tbnet::Packet * apacket, void *args);

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

    void DisConnect(uint64_t server_id);

private:
    tbnet::Transport* transport_;
    tbnet::IPacketStreamer* packet_streamer_;
    tbnet::IPacketHandler* packet_handler_;

    PacketHandlerConnToServer packet_handler_conn_to_server_;
//    PacketHandlerConnToCfgSrv packet_handler_conn_to_cfgsrv_;
    //chat with config_server
    tbnet::PacketQueueThread task_queue_thread_;


    bool stop_;
    tbsys::CThread run_thread_;
    char config_server_spec_[100];
    tbnet::Connection* conn_to_config_server_;

    //not include config_server,access_server
    std::vector<uint32_t> depend_server_type_;
    std::vector<ServerURL*> server_url_;
    //server_type---LoadConnectionManager*
    __gnu_cxx::hash_map<uint16_t,LoadConnectionManager*> conn_manager_;
    tbsys::CThreadMutex mutex_;

};


}
}

#endif