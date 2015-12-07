#include "access_server.hpp"
#include "request_packet.hpp"
#include "response_packet.hpp"
#include "access_packet.hpp"

using namespace tbnet;

namespace lljz {
namespace disk {

AccessServer::AccessServer()
:conn_manager_to_srv_(NULL) {
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

    conn_manager_to_srv_=new ConnectionManagerToServer(
        &to_server_transport_,&packet_streamer_,this);
    
    if (!conn_manager_to_srv_->start()) {
        TBSYS_LOG(ERROR,"%s","conn_manager_to_srv start error");
        Stop();
        return;
    }

    //transport
    char spec[32];
    bool ret=true;
    if (ret) {
        int port=TBSYS_CONFIG.getInt("server","port",10010);
        sprintf(spec,"tcp::%d",port);
        if (from_client_transport_.listen(spec, &packet_streamer_,this)==NULL) {
            TBSYS_LOG(ERROR,"listen port %d error",port);
            ret=false;
        } else {
            TBSYS_LOG(INFO,"listen tcp port: %d",port);
        }
    }

    if (ret) {
        TBSYS_LOG(INFO,"--- program stated PID: %d ---",getpid());
        from_client_transport_.start();
        to_server_transport_.start();
    } else {
        Stop();
    }

    task_queue_thread_.wait();
    conn_manager_to_srv_->wait();
    from_client_transport_.wait();
    to_server_transport_.wait();
    Destroy();
}

void AccessServer::Stop() {
    task_queue_thread_.stop();
    conn_manager_to_srv_->stop();
    from_client_transport_.stop();
    to_server_transport_.stop();
}

int AccessServer::Initialize() {
    //packet_streamer
    packet_streamer_.setPacketFactory(&packet_factory_);

    int thread_count = TBSYS_CONFIG.getInt(
        "server","from_client_work_thread_count",4);
    task_queue_thread_.setThreadParameter(thread_count,this,NULL);
    return EXIT_SUCCESS;
}

int AccessServer::Destroy() {
    if (conn_manager_to_srv_) {
        delete conn_manager_to_srv_;
    }
    return EXIT_SUCCESS;
}

tbnet::IPacketHandler::HPRetCode 
AccessServer::handlePacket(
tbnet::Connection *connection, 
tbnet::Packet *packet) {
    TBSYS_LOG(DEBUG,"%s",
        "AccessServer::handlePacket");

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

bool AccessServer::handlePacketQueue(
tbnet::Packet * apacket, void *args) {
    TBSYS_LOG(DEBUG,"%s",
        "AccessServer::handlePacketQueue");
    return true;
    BasePacket *packet = (BasePacket *) apacket;
    tbnet::Connection* conn=packet->get_connection();
    int64_t nowTime=tbsys::CTimeUtil::getTime();

    if (nowTime - packet->get_recv_time() > 
        PACKET_IN_PACKET_QUEUE_THREAD_MAX_TIME) {
        //PacketQueueThread中排队3min的请求丢弃
        queueThreadTimeoutThrowPackets_++;
        TBSYS_LOG(DEBUG,"queueThreadTimeoutThrowPackets_=%d",
            queueThreadTimeoutThrowPackets_);
        return true;
    }

    if (conn==NULL || conn->isConnectState()==false) {
        //失效连接上的请求丢弃
        //避免失效连接上的业务处理消耗大量的时间
        clientDisconnThrowPackets_++;
        TBSYS_LOG(DEBUG,"clientDisconnThrowPackets_=%d",
            clientDisconnThrowPackets_);
        return true;
    }

    int pcode = apacket->getPCode();
    switch (pcode) {
        case REQUEST_PACKET: {
            RequestPacket *client_req = (RequestPacket *)packet;
            AccessPacket *access_req = new AccessPacket();
            access_req->set_recv_time(tbsys::CTimeUtil::getTime());
            access_req->set_connection(client_req->get_connection());
            access_req->cli_src_type_=client_req->src_type_;
            access_req->cli_src_id_=client_req->src_id_;
            access_req->cli_chid_=client_req->getChannelId();
            access_req->src_type_=0;
            access_req->src_id_=0;
            access_req->dest_type_=client_req->dest_type_;
            access_req->dest_id_=client_req->dest_id_;
            access_req->msg_id_=client_req->msg_id_;
            access_req->version_=client_req->version_;
            strcat(access_req->data_,client_req->data_);

            //发送到业务服务器
            if (false==conn_manager_to_srv_->PostPacket(
                    access_req->dest_type_,access_req,access_req)) {
                //发送错误返回目标服务器不可达
                ResponsePacket* resp=new ResponsePacket();
                resp->setChannelId(packet->getChannelId());
                resp->setPCode(CONFIG_SERVER_GET_SERVICE_LIST_RESP);
                resp->src_id_=client_req->dest_id_;
                resp->dest_id_=client_req->src_id_;
                sprintf(resp->data_,"%s","{\"service\":[],\"ip\":\"127.0.0.1\",\"mac\":\"010A0B0C\"}");
                if (false==conn->postPacket(resp)) {
                    delete resp;
                }
                delete access_req;
            }

            delete client_req;
        }
        break;

        case RESPONSE_PACKET: {
        }
        break;
    }
    return true;
}

//收到业务应答包事件处理
bool AccessServer::BusinessHandlePacket(
tbnet::Packet *packet, void *args) {
    TBSYS_LOG(DEBUG,"%s",
        "AccessServer::BusinessHandlePacket");
    ResponsePacket* resp=NULL;
    AccessPacket *access_req=NULL;
    tbnet::Connection* conn=NULL;
    int64_t now_time=tbsys::CTimeUtil::getTime();
    if (!packet->isRegularPacket()) { // 是否正常的包
        tbnet::ControlPacket* ctrl_packet=(tbnet::ControlPacket* )packet;
        int cmd=ctrl_packet->getCommand();
        if (tbnet::ControlPacket::CMD_DISCONN_PACKET==cmd) {
            assert(false);
        } else if (tbnet::ControlPacket::CMD_TIMEOUT_PACKET==cmd) {
            access_req=(AccessPacket* )args;
            //发送错误返回目标服务器不可达
            resp=new ResponsePacket();
            resp->setChannelId(access_req->getChannelId());
            resp->setPCode(CONFIG_SERVER_GET_SERVICE_LIST_RESP);
            resp->src_id_=access_req->dest_id_;
            resp->dest_id_=access_req->src_id_;
            sprintf(resp->data_,"%s","{\"service\":[],\"ip\":\"127.0.0.1\",\"mac\":\"010A0B0C\"}");
            conn=access_req->get_connection();
            if (false==conn->postPacket(resp)) {
                delete resp;
            }
            delete access_req;
        }
        return IPacketHandler::FREE_CHANNEL;
    }

    access_req=(AccessPacket* )args;
    conn=access_req->get_connection();
    ResponsePacket* business_resp=(ResponsePacket* )packet;
    if (now_time - access_req->get_recv_time() >= PACKET_WAIT_FOR_SERVER_MAX_TIME) {
        //发送错误返回目标服务器不可达
        resp=new ResponsePacket();
        resp->setChannelId(access_req->getChannelId());
        resp->setPCode(CONFIG_SERVER_GET_SERVICE_LIST_RESP);
        resp->src_id_=access_req->dest_id_;
        resp->dest_id_=access_req->src_id_;
        sprintf(resp->data_,"%s","{\"service\":[],\"ip\":\"127.0.0.1\",\"mac\":\"010A0B0C\"}");
        if (false==conn->postPacket(resp)) {
            delete resp;
        }
        return true;
    }

    resp=new ResponsePacket();
    if (false==conn->postPacket(resp)) {
        delete resp;
    }

    return true;
}


}
}
