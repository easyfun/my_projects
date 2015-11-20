#ifndef LLJZ_DISK_RESPONSE_PACKET_H
#define LLJZ_DISK_RESPONSE_PACKET_H

#include "base_packet.hpp"

namespace lljz {
namespace disk {

#define RESPONSE_PACKET_MAX_SIZE 8192

class ResponsePacket : public BasePacket {
public:
    ResponsePacket() {
        src_id_=0;
        dest_id_=0;
        memset(data_,0,RESPONSE_PACKET_MAX_SIZE);
    }

    ~ResponsePacket() {

    }

    bool encode(tbnet::DataBuffer *output) {
        output->writeInt32(src_id_);
        output->writeInt32(dest_id_);
        output->writeBytes(data_, strlen(data_));
        return true;
    }

    bool decode(tbnet::DataBuffer *input, tbnet::PacketHeader *header) 
    {
        if (header->_dataLen < 8) {
            TBSYS_LOG(ERROR,"%s\n", "buffer data too few.");
            return false;
        }

        if (header->_dataLen > RESPONSE_PACKET_MAX_SIZE) {
            TBSYS_LOG(ERROR,"%s\n", "buffer data too more.");
            return false;
        }

        src_id_=input->readInt32();
        dest_id_=input->readInt32();
        if (!input->readBytes(data_,header->_dataLen-8)) {
            return false;
        }
        data_[header->_dataLen-8]='\0';
        return true;
    }

    void free() {
    }

public:
    uint32_t src_id_;   //源服务id
    uint32_t dest_id_;  //目标服务id
    //消息id，继承自Packet::_packetHeader._pcode
    //请求id，继承自Packet::_packetHeader._chid
    //uint32_t version_;  //消息版本号
    char data_[RESPONSE_PACKET_MAX_SIZE]; //
};

}
}

#endif
