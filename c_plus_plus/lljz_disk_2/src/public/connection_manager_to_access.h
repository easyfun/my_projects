#ifndef LLJZ_DISK_CONNECTION_MANAGER_TO_ACCESS_H_
#define LLJZ_DISK_CONNECTION_MANAGER_TO_ACCESS_H_

namespace lljz {
namespace disk {

typedef __gnu_cxx::hash_map<uint64_t,tbnet::Connection*> AccessConnectionMap;

class ConnectionManagerToAccess:public tbnet::IPacketHandler {
public:
    ConnectionManagerToAccess();
    ~ConnectionManagerToAccess();

    //IPacketHandler interface
    tbnet::IPacketHandler::HPRetCode 
        handlePacket(tbnet::Packet *packet, void *args);

    bool start();
    bool stop();
    bool wait();
    void destroy();

    void setDefaultQueueLimit(int queueLimit) {
        _queueLimit=queueLimit;
    }

    void setDefaultQueueTimeout(int queueTimeout) {
        _queueTimeout=queueTimeout;
    }

    //Post packet to server
    bool postPacket(uint64_t server_id,
        tbnet::Packet* packet, 
        void* args=NULL);

private:
    PacketFactory packet_factory_;
    tbnet::DefaultPacketStreamer packet_streamer_;
    tbnet::Transport transport_for_access_;
    
    int _queueLimit;
    int _queueTimeout;

    AccessConnectionMap conn_map_;
};

}
}

#endif