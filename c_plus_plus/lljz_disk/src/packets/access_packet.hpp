#ifndef LLJZ_DISK_ACCESS_PACKET_H
#define LLJZ_DISK_ACCESS_PACKET_H

#include "base_packet.hpp"
#include "request_packet.hpp"

namespace lljz {
namespace disk {

class AccessPacket : public RequestPacket {
public:
    AccessPacket() {
        cli_src_type_=0;
        cli_src_id_=0;
        cli_chid_=0;
    }

    ~AccessPacket() {

    }

    void free() {
    }

public:
    //客户端原始Packet关键字段
    uint16_t cli_src_type_;
    uint64_t cli_src_id_;
    uint32_t cli_chid_;
};

}
}

#endif