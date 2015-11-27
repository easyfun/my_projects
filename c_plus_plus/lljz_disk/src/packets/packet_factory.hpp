#ifndef LLJZ_DISK_PACKET_FACTORY_H
#define LLJZ_DISK_PACKET_FACTORY_H

#include "base_packet.hpp"
#include "request_packet.hpp"
#include "response_packet.hpp"

namespace lljz {
namespace disk {  

class PacketFactory : public tbnet::IPacketFactory {
public:
    PacketFactory() {}
    ~PacketFactory() {}

    tbnet::Packet *createPacket(int pcode) {
        if (RESPONSE_PACKET==pcode)
            return new ResponsePacket();
        return new RequestPacket();
    }
};

}
}
#endif
