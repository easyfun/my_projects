#include "packet_handler_to_server.hpp"
#include "connection_manager_to_server.hpp"
#include "base_packet.hpp"
#include "request_packet.hpp"
#include "response_packet.hpp"

namespace lljz {
namespace disk {

bool PacketHandlerToServer::start() {
    int thread_count = TBSYS_CONFIG.getInt("server",
        "to_server_work_thread_count",1);
    task_queue_thread_.setThreadParameter(thread_count,this,NULL);
    task_queue_thread_.start();
    return true;
}

bool PacketHandlerToServer::stop() {
    TBSYS_LOG(DEBUG,"%s",
        "PacketHandlerToServer::stop");
    task_queue_thread_.stop();
    return true;
}

bool PacketHandlerToServer::wait() {
    TBSYS_LOG(DEBUG,"%s",
        "PacketHandlerToServer::wait");
    task_queue_thread_.wait();
    return true;
}

tbnet::IPacketHandler::HPRetCode 
PacketHandlerToServer::handlePacket(
tbnet::Packet *packet, void *args) {
    TBSYS_LOG(DEBUG,"%s",
        "PacketHandlerToServer::handlePacket");
    if (!packet->isRegularPacket()) { // 是否正常的包
        tbnet::ControlPacket* ctrl_packet=(tbnet::ControlPacket* )packet;
        int cmd=ctrl_packet->getCommand();
        //连接断开事件通知给连接管理器处理
        if (tbnet::ControlPacket::CMD_DISCONN_PACKET==cmd) {
/*            tbnet::Socket* socket=(tbnet::Socket* )args;
            //通知到业务服务连接断开
            //暂时LoadConnectionManager管理
            //pending call
            //needing post in pri task queue
            conn_manager_to_srv_->DisToReConnect(socket->getId());*/
            assert(false);
            return tbnet::IPacketHandler::FREE_CHANNEL;
        } else if (tbnet::ControlPacket::CMD_TIMEOUT_PACKET==cmd) {
/*            packet->set_args(args);
            task_queue_thread_.push(packet);*/
        TBSYS_LOG(DEBUG,"%s",
            "PacketHandlerToServer::handlePacket:CMD_TIMEOUT_PACKET");
        } else {
            assert(false);
        }
    } else {
        BasePacket* bp=(BasePacket* )packet;
        bp->set_args(args);
        task_queue_thread_.push(packet);
    }
    return IPacketHandler::FREE_CHANNEL;
}

bool PacketHandlerToServer::handlePacketQueue(
tbnet::Packet * apacket, void *args) {
    TBSYS_LOG(DEBUG,"%s",
        "PacketHandlerToServer::handlePacketQueue");
    ResponsePacket* resp=(ResponsePacket* )apacket;
    RequestPacket* req=(RequestPacket* )resp->get_args();
    
    if (SERVER_TYPE_CONFIG_SERVER==req->dest_type_
    && CONFIG_SERVER_GET_SERVICE_LIST_REQ==req->msg_id_) {
        conn_manager_to_srv_->GetServiceListResp(
            resp, resp->get_args());
        return true;
    }

    if (business_packet_handler_) {
        business_packet_handler_->BusinessHandlePacket(
            resp, resp->get_args());
    }

    return true;
}



}
}