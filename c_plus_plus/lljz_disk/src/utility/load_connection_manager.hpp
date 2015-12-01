#ifndef LLJZ_DISK_LOAD_CONNECTION_MANAGER_H
#define LLJZ_DISK_LOAD_CONNECTION_MANAGER_H

#include "tbnet.h"
#include "tbsys.h"

namespace lljz {
namespace disk {

typedef __gnu_cxx::hash_map<uint64_t, Connection*, __gnu_cxx::hash<int> > TBNET_CONN_MAP;

struct ReconnectInfo {
    uint64_t server_id_;
    int status_;

    ReconnectInfo(uint64_t server_id) {
        server_id_=server_id;
        status_=0;//1-正在连接
    }
};

class LoadConnectionManager:public IPacketHandler {
public:
    LoadConnectionManager(tbnet::Transport *transport, 
        tbnet::IPacketStreamer *streamer, 
        tbnet::IPacketHandler *packetHandler);
    ~LoadConnectionManager();

    //IPacketHandler
    tbnet::HPRetCode handlePacket(Packet *packet, void *args);

    tbnet::Connection *connect(uint64_t serverId, 
        bool autoConn=false);
    //server_id断开不重连，从依赖的后端服务移出
    void disconnect(uint64_t serverId);
    void setDefaultQueueLimit(int queueLimit);
    void setDefaultQueueTimeout(int queueTimeout);
    void cleanup();
    bool sendPacket(tbnet::Packet* packet, 
        tbnet::IPacketHandler *packetHandler = NULL, 
        void *args = NULL, bool noblocking = true);

    void CheckReconnect();
    //server_id断开自动重连
    void DisToReconnect(uint64_t server_id);

private:
//    uint32_t server_type_;
    Transport *_transport;
    IPacketStreamer *_streamer;
    IPacketHandler *_packetHandler;
    int _queueLimit;
    int _queueTimeout;
    int _status;

    TBNET_CONN_MAP _connectMap;
//    tbsys::CThreadMutex _mutex;
    std::vector<uint64_t> server_id_;
    atomic_t last_server_id_index_;
    tbsys::CRWSimpleLock rw_simple_lock_;

    std::vector<uint64_t> reconn_server_id_;
    tbsys::CThreadMutex reconn_mutex_;
};

}
}

#endif