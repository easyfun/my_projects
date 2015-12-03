#ifndef LLJZ_DISK_LOAD_CONNECTION_MANAGER_H
#define LLJZ_DISK_LOAD_CONNECTION_MANAGER_H

#include "tbnet.h"
#include "tbsys.h"

namespace lljz {
namespace disk {

typedef __gnu_cxx::hash_map<uint64_t, 
    tbnet::Connection*, 
    __gnu_cxx::hash<int> > TBNET_CONN_MAP;

struct ReconnectInfo {
    uint64_t server_id_;
    int status_;

    ReconnectInfo(uint64_t server_id) {
        server_id_=server_id;
        status_=0;//1-正在连接
    }
};

class LoadConnectionManager:public tbnet::IPacketHandler {
public:
    LoadConnectionManager(tbnet::Transport* transport, 
        tbnet::IPacketStreamer *streamer, 
        tbnet::IPacketHandler *packetHandler=NULL);
    ~LoadConnectionManager();

    //IPacketHandler
    //阻塞IO loop 优先级任务，待进一步优化
    //disconnect not proc
    tbnet::IPacketHandler::HPRetCode 
    handlePacket(tbnet::Packet *packet, void *args);

    //增加连接
    tbnet::Connection *connect(uint64_t serverId, 
        bool autoConn=false);
    //移除连接
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
    tbnet::Transport *_transport;
    tbnet::IPacketStreamer *_streamer;
    //disconnect register handler
    IPacketHandler *_packetHandler;
    int _queueLimit;
    int _queueTimeout;
    int _status;

    //连接状态
    //disconnect --> connect --> register --> send
    //--> timeout
    //--> disconnect
    //发送连接
    TBNET_CONN_MAP send_conn_map_;
    std::vector<uint64_t> send_server_id_;
    atomic_t last_send_server_id_index_;
    tbsys::CRWSimpleLock send_conn_rw_lock_;

    //建立连接
    std::vector<uint64_t> reconn_server_id_; //重连队列
    std::vector<uint64_t> inconn_server_id_; //正在连接队列（注册srv_id）
    tbsys::CThreadMutex create_conn_mutex_; //建立连接共享锁
};

}
}

#endif