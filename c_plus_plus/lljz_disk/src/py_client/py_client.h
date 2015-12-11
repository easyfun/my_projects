#ifndef LLJZ_DISK_PY_CLIENT_H
#define LLJZ_DISK_PY_CLIENT_H

#include "tbsys.h"
#include "tbnet.h"
#include "packet_factory.hpp"
#include "response_packet.hpp"

namespace lljz {
namespace disk {

struct ConnectCond {
    tbnet::Connection* conn_;
    tbsys::CThreadCond cond_;
    int send_;
    ResponsePacket resp_;

    ConnectCond() {
        conn_=NULL;
        send_=0;
    }
};

typedef __gnu_cxx::hash_map<uint64_t,
            tbnet::Connection*> ConnectMap;

class PyClient:public tbsys::Runnable,
public tbnet::IPacketHandler {
public:
    PyClient();
    ~PyClient();

    //Runnable interface
    void run(tbsys::CThread* thread, void* arg);

    //IPacketHandler interface
    tbnet::IPacketHandler::HPRetCode 
    handlePacket(tbnet::Packet *packet, void *args);

    bool start();
    bool stop();
    bool wait();
    void destroy();

    uint64_t Connect(std::string spec, bool auto_conn);
    void Disconnect(uint64_t conn_id);
    std::string Send(uint64_t conn_id, std::string req_str);
//    std::string Send(std::string req_str);    

private:
    PacketFactory packet_factory_;
    tbnet::DefaultPacketStreamer packet_streamer_;
    tbnet::Transport transport_;

    bool stop_;
    tbsys::CThread loop_thread_;

    ConnectMap conn_map_;
    tbsys::CThreadMutex mutex_;
};

}
}
#endif