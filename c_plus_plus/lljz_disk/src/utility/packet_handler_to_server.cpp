#include "pakcet_handler_to_server.hpp"
#include "connection_manager_to_server.hpp"

namespace lljz {
namespace disk {

bool PacketHandlerToServer::start() {
    int thread_count = TBSYS_CONFIG.getInt("server","to_server_work_thread_count",4);
    task_queue_thread_.setThreadParameter(thread_count,this,NULL);
    task_queue_thread_.start();
}

bool PacketHandlerToServer::stop() {
    task_queue_thread_.stop();
}

bool PacketHandlerToServer::wait() {
    task_queue_thread_.wait();
}

tbnet::HPRetCode PacketHandlerToServer::handlePacket(
tbnet::Packet *packet, void *args) {
    if (!packet->isRegularPacket()) { // 是否正常的包
        tbnet::ControlPacket* ctrl_packet=(tbnet::ControlPacket* )packet;
        int cmd=ctrl_packet->getCommand();
        //连接断开事件通知给连接管理器处理
        if (tbnet::ControlPacket::CMD_DISCONN_PACKET==cmd) {
            tbnet::Socket* socket=(tbnet::Socket* )args;
            //通知连接无效
            conn_manager_to_srv_->DisToReConnect(socket->getId());
            return IPacketHandler::FREE_CHANNEL;
        } else if (tbnet::ControlPacket::CMD_TIMEOUT_PACKET==cmd) {
            packet->set_args(args);
            task_queue_thread_.push(packet);
        } else {
            assert(false);
        }
    } else {
        packet->set_args(args);
        task_queue_thread_.push(packet);
    }
    return IPacketHandler::FREE_CHANNEL;
}

bool PacketHandlerToServer::handlePacketQueue(
tbnet::Packet * apacket, void *args) {
    ResponsePacket* resp=(ResponsePacket* )apacket;
    RequestPacket* req=(RequestPacket* )resp->get_args();
    
    if (SERVER_TYPE_CONFIG_SERVER==req->dest_type_
    && CONFIG_SERVER_GET_SERVICE_LIST_RESP==req->msg_id_) {
        conn_manager_to_srv_->GetServiceListResp(
            resp, resp->get_args());
    }

    if (business_packet_handler_) {
        business_packet_handler_->BusinessHandlePacket(
            resp, resp->get_args());
    }

    return true;
}



}
}