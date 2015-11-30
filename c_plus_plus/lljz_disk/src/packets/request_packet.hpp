#ifndef LLJZ_DISK_REQUEST_PACKET_H
#define LLJZ_DISK_REQUEST_PACKET_H

#include "base_packet.hpp"

namespace lljz {
namespace disk {

#define REQUEST_PACKET_HEAD_LEN 16
#define REQUEST_PACKET_MAX_SIZE 4096

class RequestPacket : public BasePacket {
public:
    RequestPacket() {
        setPCode(REQUEST_PACKET);
        src_id_=0;
        dest_id_=0;
        msg_id_=0;
        version_=0;
        memset(data_,0,REQUEST_PACKET_MAX_SIZE);
    }

    ~RequestPacket() {

    }

    bool encode(tbnet::DataBuffer *output) {
        output->writeInt32(src_id_);
        output->writeInt32(dest_id_);
        output->writeInt32(msg_id_);
        output->writeInt32(version_);
        output->writeBytes(data_, strlen(data_));
        return true;
    }

    bool decode(tbnet::DataBuffer *input, tbnet::PacketHeader *header) 
    {
        if (header->_dataLen < REQUEST_PACKET_HEAD_LEN) {
            TBSYS_LOG(ERROR,"%s\n", "buffer data too few.");
            return false;
        }

        if (header->_dataLen > REQUEST_PACKET_MAX_SIZE) {
            TBSYS_LOG(ERROR,"%s\n", "buffer data too more.");
            return false;
        }

        src_id_=input->readInt32();
        dest_id_=input->readInt32();
        msg_id_=input->readInt32();
        version_=input->readInt32();
        if (!input->readBytes(data_,header->_dataLen-REQUEST_PACKET_HEAD_LEN)) {
            return false;
        }
        data_[header->_dataLen-REQUEST_PACKET_HEAD_LEN]='\0';
        return true;
    }

    void free() {
    }

public:
    uint16_t src_type_; //源类型
    uint64_t src_id_;   //源id
    uint16_t dest_type_;//目标类型
    uint64_t dest_id_;  //目标id
    uint32_t msg_id_;   //消息id
    //请求id，继承自Packet::_packetHeader._chid
    uint32_t version_;  //消息版本号
    char data_[REQUEST_PACKET_MAX_SIZE]; //
};

}
}

#endif
