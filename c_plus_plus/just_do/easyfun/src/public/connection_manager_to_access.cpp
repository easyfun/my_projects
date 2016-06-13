#include "tbnet.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace tbnet;
using namespace tbsys;
using namespace rapidjson;

#include "protocol.hpp"
#include "base_packet.hpp"
#include "packet_factory.hpp"
#include "request_packet.hpp"
#include "response_packet.hpp"
#include "global.h"
#include "connection_manager_to_access.h"

namespace lljz {
namespace disk {
ConnectionManagerToAccess::ConnectionManagerToAccess() {
    _queueLimit=0;
    _queueTimeout=3000;//3000ms
}

ConnectionManagerToAccess::~ConnectionManagerToAccess() {

}

bool ConnectionManagerToAccess::start() {
    //packet_streamer
    packet_streamer_.setPacketFactory(&packet_factory_);
    transport_for_access_.start();

    //依赖接入服务
    std::vector<const char*> access_specs=
        TBSYS_CONFIG.getStringList("access_server", "access_spec");
    if (access_specs.size() <= 0) {
        TBSYS_LOG(ERROR,"server_type_num == 0");
        return false;
    }

    tbnet::Connection* conn;
    uint64_t server_id;
    for (int i=0;i<access_specs.size();i++) {
        conn=transport_for_access_.connect(access_specs[i],
            &packet_streamer_,true);
        if (NULL==conn) {
            TBSYS_LOG(ERROR,"connect to access-%s fail", access_specs[i]);
            return false;
        }
        conn->setDefaultPacketHandler(this);
        conn->setQueueLimit(_queueLimit);
        conn->setQueueTimeout(_queueTimeout);

        server_id=GetServerID(access_specs[i]);
        assert(server_id==conn->getServerId());
        conn_map_[conn->getServerId()]=conn;
    }

    return true;
}

bool ConnectionManagerToAccess::stop() {
    transport_for_access_.stop();
    return true;
}

bool ConnectionManagerToAccess::wait() {
    transport_for_access_.wait();
    destroy();
    return true;
}

void ConnectionManagerToAccess::destroy() {
/*    AccessConnectionMap::iterator it;
    for (it=conn_map_.begin();it!=conn_map_.end();it++) {

    }
*/
    conn_map_.clear();
}

//Post packet to server
bool ConnectionManagerToAccess::postPacket(uint64_t server_id,
tbnet::Packet* packet, void* args) {
    AccessConnectionMap::iterator it;
    it=conn_map_.find(server_id);
    if (conn_map_.end() == it) {
        return false;
    }
    tbnet::Connection* conn = it->second;
    if (!conn) {
        return false;
    }
    return conn->postPacket(packet, this, args);
}


tbnet::IPacketHandler::HPRetCode 
ConnectionManagerToAccess::handlePacket(
tbnet::Packet *packet, void *args) {
    if (!packet->isRegularPacket()) { // 是否正常的包
        tbnet::ControlPacket* ctrl_packet=(tbnet::ControlPacket* )packet;
        int cmd=ctrl_packet->getCommand();
        if (tbnet::ControlPacket::CMD_DISCONN_PACKET==cmd) {
            TBSYS_LOG(DEBUG,"%s",
                "ManagerServer::handlePacket:CMD_DISCONN_PACKET");
        } else if (tbnet::ControlPacket::CMD_TIMEOUT_PACKET==cmd) {
            TBSYS_LOG(DEBUG,"%s",
                "ManagerServer::handlePacket:CMD_TIMEOUT_PACKET");
            //非业务超时，即通讯断开超时，在业务超时时间内
            
        } else {
            assert(false);
        }
        return tbnet::IPacketHandler::FREE_CHANNEL;
    }

    // 接入<---->业务通讯
    // 发送、接收不共用连接
    // 向接入服务器分发应答通道
    // 接入不回复包
    // 接入与业务服务器不用维护连接状态，Connection上可以使用自动重连
    assert(false);
    return tbnet::IPacketHandler::FREE_CHANNEL;
}


}
}