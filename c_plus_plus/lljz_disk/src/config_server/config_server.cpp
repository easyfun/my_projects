#include "config_server.hpp"
#include "request_packet.hpp"
#include "response_packet.hpp"

using namespace tbnet;

namespace lljz {
namespace disk {

ConfigServer::ConfigServer() {

}

ConfigServer::~ConfigServer() {

}

void ConfigServer::Start() {
    if (Initialize()) {
        return;
    }

    //process thread
    task_queue_thread_.start();

    //transport
    char spec[32];
    bool ret=true;
    if (ret) {
        int port=TBSYS_CONFIG.getInt("config_server","port",10000);
        sprintf(spec,"tcp::%d",port);
        if (packet_transport_.listen(spec, &packet_streamer_,this)==NULL) {
            TBSYS_LOG(ERROR,"listen port %d error",port);
            ret=false;
        } else {
            TBSYS_LOG(INFO,"listen tcp port: %d",port);
        }
    }

    if (ret) {
        TBSYS_LOG(INFO,"--- program stated PID: %d ---",getpid());
        packet_transport_.start();
    } else {
        Stop();
    }

    task_queue_thread_.wait();
    packet_transport_.wait();

    Destroy();
}

void ConfigServer::Stop() {
    task_queue_thread_.stop();
    packet_transport_.stop();
}

tbnet::IPacketHandler::HPRetCode ConfigServer::handlePacket(tbnet::Connection *connection, tbnet::Packet *packet) {
    if (!packet->isRegularPacket()) {
/*        TBSYS_LOG(ERROR,"ControlPacket, cmd: %d",
            (tbnet::ControlPacket* )packet->getCommand());
*/
        return IPacketHandler::FREE_CHANNEL;
    }

    BasePacket* bp=(BasePacket* )packet;
    bp->set_connection(connection);
    bp->set_direction(DIRECTION_RECEIVE);
    task_queue_thread_.push(bp);

    return tbnet::IPacketHandler::KEEP_CHANNEL;
}

bool ConfigServer::handlePacketQueue(tbnet::Packet * apacket, void *args) {
    BasePacket *packet = (BasePacket *) apacket;
    int pcode = apacket->getPCode();

    switch (pcode) {
        case CONFIG_SERVER_GET_SERVICE_LIST_REQ: {
            RequestPacket *req = (RequestPacket *) packet;
            ResponsePacket *resp = new ResponsePacket();
            resp->setChannelId(packet->getChannelId());
            resp->setPCode(CONFIG_SERVER_GET_SERVICE_LIST_RESP);
            resp->src_id_=req->dest_id_;
            resp->dest_id_=req->src_id_;
            sprintf(resp->data_,"%s","{\"service\":[],\"ip\":\"127.0.0.1\",\"mac\":\"010A0B0C\"}");
            if(packet->get_connection()->postPacket(resp) == false) {
                delete resp;
            }
        }
        break;
    }
    return true;
}

int ConfigServer::Initialize() {
    //packet_streamer
    packet_streamer_.setPacketFactory(&packet_factory_);

    int thread_count = TBSYS_CONFIG.getInt("config_server","work_thread_count",4);
    task_queue_thread_.setThreadParameter(thread_count,this,NULL);
    return EXIT_SUCCESS;
}

int ConfigServer::Destroy() {
    return EXIT_SUCCESS;
}

}
}
