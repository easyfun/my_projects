#include "manager_server.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "base_packet.hpp"
#include "request_packet.hpp"
#include "response_packet.hpp"
#include "global.h"

using namespace rapidjson;

namespace lljz {
namespace disk {

ManagerServer::ManagerServer()
： stop_(false) {
}

ManagerServer::~ManagerServer() {
}

bool ManagerServer::start() {

    const char * str_config_value;

    self_server_type_=TBSYS_CONFIG.getInt(
        "server","self_server_type",
        SERVER_TYPE_ACCESS_SERVER);

    str_config_value=TBSYS_CONFIG.getString(
        "server","self_server_spec","");
    if (0==strlen(str_config_value)) {
        TBSYS_LOG(ERROR,"%s",
            "config error,self_server_spec error");
        return false;
    }
    memset(self_server_spec_,0,
        sizeof(self_server_spec_));
    strcat(self_server_spec_,str_config_value);

    self_server_id_=GetServerID(self_server_spec_);
    if (0==self_server_id_) {
        TBSYS_LOG(ERROR,"%s",
            "config error,self_server_spec error"
            ",self_server_id error");
        return false;
    }

    int thread_count = TBSYS_CONFIG.getInt(
        "server","work_thread_count_for_server",4);
    task_queue_thread_.setThreadParameter(thread_count,this,NULL);
    
    task_queue_thread_.start();
    //timer_thread_.start(this,NULL);
    packet_streamer_.setPacketFactory(&packet_factory_);
    transport_for_server_.start();

    return true;
}

bool ManagerServer::stop() {
    TBSYS_LOG(DEBUG,"%s",
        "ManagerServer::stop");
    stop_=true;
    transport_for_server_.stop();
    task_queue_thread_.stop();
    return true;
}

bool ManagerServer::wait() {
    task_queue_thread_.wait();
    transport_for_server_.wait();
    //timer_thread_.join();
    destroy();
    return true;
}

void ManagerServer::destroy() {

}

/*
void ManagerServer::run(
tbsys::CThread* thread, void* arg) {
    while (!stop_) {
        usleep(1000000);//1s
    }
}
*/

bool ManagerServer::PostPacket(
uint16_t server_type,
tbnet::Packet* packet, 
void* args) {
    return true;
}

tbnet::IPacketHandler::HPRetCode 
ManagerServer::handlePacket(
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
        } else {
            assert(false);
        }
        return IPacketHandler::FREE_CHANNEL;
    }

    // 发送、接收不共用连接
    // 向业务服务器分发请求通道
    // 接入与业务服务器不用维护连接状态，Connection上可以使用自动重连
    assert(false);
    return IPacketHandler::FREE_CHANNEL;
}

/*
bool ManagerServer::handlePacketQueue(
tbnet::Packet * apacket, void *args) {
    return true;
}
*/

}//end disk
}//end lljz