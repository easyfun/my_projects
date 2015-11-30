#include "access_server.hpp"
#include "request_packet.hpp"
#include "response_packet.hpp"

using namespace tbnet;

namespace lljz {
namespace disk {

AccessServer::AccessServer() {
    clientDisconnThrowPackets_=0;
    queueThreadTimeoutThrowPackets_=0;
}

AccessServer::~AccessServer() {

}

void AccessServer::Start() {
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

void AccessServer::Stop() {
    task_queue_thread_.stop();
    packet_transport_.stop();
}

tbnet::IPacketHandler::HPRetCode AccessServer::handlePacket(tbnet::Connection *connection, tbnet::Packet *packet) {
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

bool AccessServer::handlePacketQueue(tbnet::Packet * apacket, void *args) {
    BasePacket *packet = (BasePacket *) apacket;
    tbnet::Connection* conn=packet->get_connection();
    int64_t nowTime=tbsys::CTimeUtil::getTime();

    if (nowTime - packet->get_recv_time() > PACKET_IN_PACKET_QUEUE_THREAD_MAX_TIME) {
        //PacketQueueThread中排队3min的请求丢弃
        queueThreadTimeoutThrowPackets_++;
        TBSYS_LOG(DEBUG,"queueThreadTimeoutThrowPackets_=%d",queueThreadTimeoutThrowPackets_);
        return true;
    }

    if (conn==NULL || conn->isConnectState()==false) {
        //失效连接上的请求丢弃
        //避免失效连接上的业务处理消耗大量的时间
        clientDisconnThrowPackets_++;
        TBSYS_LOG(DEBUG,"clientDisconnThrowPackets_=%d",clientDisconnThrowPackets_);
        return true;
    }

    int pcode = apacket->getPCode();
    switch (pcode) {
        case REQUEST_PACKET: {
            RequestPacket *clientReq = (RequestPacket * ) packet;
            AccessPacket *accessReq = new AccessPacket();
            accessReq->set_recv_time(tbsys::CTimeUtil::getTime());
            accessReq->set_connection(clientReq->get_connection());
            accessReq->cli_src_type_=clientReq->src_type_;
            accessReq->cli_src_id_=clientReq->src_id_;
            accessReq->cli_chid_=clientReq->getChannelId();
            accessReq->src_type_=0;
            accessReq->src_id_=0;
            accessReq->dest_type_=clientReq->dest_type_;
            accessReq->dest_id_=clientReq->dest_id_;
            accessReq->msg_id_=clientReq->msg_id_;
            accessReq->version_=clientReq->version_;
            strcat(accessReq->data_,clientReq->data_);

            //发送到业务服务器
            //发送错误返回目标服务器不可达
            if(conn->postPacket(accessReq) == false) {
                delete accessReq;
            }

            // 传accessReq, args=accessReq
            // 不用维护接入服务分配的chid与客户端连接映射关系

        }
        break;

        case RESPONSE_PACKET: {
        }
        break;
    }
    return true;
}

int AccessServer::Initialize() {
    //packet_streamer
    packet_streamer_.setPacketFactory(&packet_factory_);

    int thread_count = TBSYS_CONFIG.getInt("config_server","work_thread_count",4);
    task_queue_thread_.setThreadParameter(thread_count,this,NULL);
    return EXIT_SUCCESS;
}

int AccessServer::Destroy() {
    return EXIT_SUCCESS;
}

}
}
