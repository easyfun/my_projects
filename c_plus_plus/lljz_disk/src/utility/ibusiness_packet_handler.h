#ifndef LLJZ_DISK_IBUSINESS_PACKET_HANDLER_H
#define LLJZ_DISK_IBUSINESS_PACKET_HANDLER_H

#include "tbsys.h"
#include "tbnet.h"

class IBusinessPacketHandler {
public:
    virtual bool BusinessHandlePacket(
        tbnet::Packet *packet, void *args) = 0;
};

#endif