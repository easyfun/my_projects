#ifndef LLJZ_DISK_PACKET_HANDLER_TO_SERVER_H
#define LLJZ_DISK_PACKET_HANDLER_TO_SERVER_H

#include "tbnet.h"
#include "tbsys.h"
#include "ibusiness_packet_handler.h"

namespace lljz {
namespace disk {

class ConnectionManagerToServer;
class PacketHandlerToServer:public tbnet::IPacketHandler,
public tbnet::IPacketQueueHandler {
public:
    PacketHandlerToServer(ConnectionManagerToServer* cmts,
    IBusinessPacketHandler* bph) {
        conn_manager_to_srv_=cmts;
        business_packet_handler_=bph;
    }

    ~PacketHandlerToServer() {

    }

    //IPacketHandler interface
    tbnet::IPacketHandler::HPRetCode 
    handlePacket(tbnet::Packet *packet, void *args);

    // IPacketQueueHandler interface
    bool handlePacketQueue(tbnet::Packet * apacket, void *args);

    bool start();
    bool stop();
    bool wait();


private:
    ConnectionManagerToServer* conn_manager_to_srv_;

    tbnet::PacketQueueThread task_queue_thread_;
    IBusinessPacketHandler* business_packet_handler_;
};


}
}

#endif