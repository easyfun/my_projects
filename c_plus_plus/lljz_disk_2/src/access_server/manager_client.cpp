#include "manager_client.h"
#include "request_packet.hpp"
#include "response_packet.hpp"

using namespace tbnet;

namespace lljz {
namespace disk {

ManagerClient::ManagerClient() {
    client_disconn_throw_packets_=0;
    queue_thread_timeout_throw_packets_=0;
}

ManagerClient::~ManagerClient() {
}

void ManagerClient::Start() {
    if (Initialize()) {
        TBSYS_LOG(ERROR,"Initialize fail");
        return;
    }

    //process thread
    task_queue_thread_.start();

    //transport
    char spec[32];
    bool ret=true;
    if (ret) {
        int port=TBSYS_CONFIG.getInt("server","port",10010);
        sprintf(spec,"tcp::%d",port);
        if (transport_for_client_.listen(spec, &packet_streamer_,this)==NULL) {
            TBSYS_LOG(ERROR,"listen port %d error",port);
            ret=false;
        } else {
            TBSYS_LOG(INFO,"listen tcp port: %d",port);
        }
    }

    if (ret) {
        TBSYS_LOG(INFO,"--- program stated PID: %d ---",getpid());
        transport_for_client_.start();
    } else {
        Stop();
    }

    task_queue_thread_.wait();
    transport_for_client_.wait();
    Destroy();
}

void ManagerClient::Stop() {
    task_queue_thread_.stop();
    transport_for_client_.stop();
}

int ManagerClient::Initialize() {
    //packet_streamer
    packet_streamer_.setPacketFactory(&packet_factory_);

    int thread_count = TBSYS_CONFIG.getInt(
        "server","from_client_work_thread_count",4);
    task_queue_thread_.setThreadParameter(thread_count,this,NULL);

/*    self_server_type_=TBSYS_CONFIG.getInt(
        "server","self_server_type",
        SERVER_TYPE_ACCESS_SERVER);
*/
    const char* str_config_value=TBSYS_CONFIG.getString(
        "server","self_server_spec","");
    if (0==strlen(str_config_value)) {
        TBSYS_LOG(ERROR,"%s",
            "config error,self_server_spec error");
        return EXIT_FAILURE;
    }
    memset(self_server_spec_,0,
        sizeof(self_server_spec_));
    strcat(self_server_spec_,str_config_value);

    self_server_id_=GetServerID(self_server_spec_);
    if (0==self_server_id_) {
        TBSYS_LOG(ERROR,"%s",
            "config error,self_server_spec error"
            ",self_server_id error");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int ManagerClient::Destroy() {
    return EXIT_SUCCESS;
}

tbnet::IPacketHandler::HPRetCode 
ManagerClient::handlePacket(
tbnet::Connection *connection, 
tbnet::Packet *packet) {
    if (!packet->isRegularPacket()) {
        TBSYS_LOG(ERROR,"ControlPacket, cmd: %d",
            ((tbnet::ControlPacket* )packet)->getCommand());

        return IPacketHandler::FREE_CHANNEL;
    }

    BasePacket* bp=(BasePacket* )packet;
    bp->set_recv_time(tbsys::CTimeUtil::getTime());
    bp->set_connection(connection);
    bp->set_direction(DIRECTION_RECEIVE);
    task_queue_thread_.push(bp);

    return tbnet::IPacketHandler::KEEP_CHANNEL;
}

bool ManagerClient::handlePacketQueue(
tbnet::Packet * apacket, void *args) {
    BasePacket *packet = (BasePacket *) apacket;
    tbnet::Connection* conn=packet->get_connection();
    int64_t nowTime=tbsys::CTimeUtil::getTime();

    if (nowTime - packet->get_recv_time() > 
        PACKET_IN_PACKET_QUEUE_THREAD_MAX_TIME) {
        //PacketQueueThread中排队3min的请求丢弃
        queue_thread_timeout_throw_packets_++;
        TBSYS_LOG(DEBUG,"queue_thread_timeout_throw_packets_=%d",
            queue_thread_timeout_throw_packets_);
        return true;
    }

    if (conn==NULL || conn->isConnectState()==false) {
        //失效连接上的请求丢弃
        //避免失效连接上的业务处理消耗大量的时间
        client_disconn_throw_packets_++;
        TBSYS_LOG(DEBUG,"client_disconn_throw_packets_=%d",
            client_disconn_throw_packets_);
        return true;
    }

    int pcode = apacket->getPCode();
    switch (pcode) {
        case REQUEST_PACKET: {
            RequestPacket *client_req = (RequestPacket *)packet;

            TBSYS_LOG(DEBUG,"client_req:chanid=%u|pcode=%u|msg_id=%u"
                "version=%u|src_type=%u|"
                "src_id=%llu|dest_type=%u|dest_id=%u|data=%s",
                client_req->getChannelId(),client_req->getPCode(),
                client_req->msg_id_,client_req->version_,
                client_req->src_type_,
                client_req->src_id_,client_req->dest_type_,
                client_req->dest_id_,client_req->data_);

            //发送到业务服务器
        }
        break;

        case RESPONSE_PACKET: {
        }
        break;
    }
    return true;
}


}
}
