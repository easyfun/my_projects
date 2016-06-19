#include "shared.h"

namespace lljz {
namespace disk {

ManagerClient::ManagerClient() {
    clientDisconnThrowPackets_=0;
    queueThreadTimeoutThrowPackets_=0;
}

ManagerClient::~ManagerClient() {
}

bool ManagerClient::start() {
    if (initialize()) {
        return false;
    }

    if (!initHandler()) {
        TBSYS_LOG(ERROR,"%s","init handler start error");
        return false;
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
        return false;
    }

    if (!conn_manager_to_access_.start()) {
        return false;
    }

    return true;
}

bool ManagerClient::stop() {
    task_queue_thread_.stop();
    transport_for_client_.stop();
    conn_manager_to_access_.stop();
    stopHandler();
    return true;
}

bool ManagerClient::wait() {
    task_queue_thread_.wait();
    transport_for_client_.wait();
    conn_manager_to_access_.wait();
    waitHandler();
    destroy();
    return true;
}

int ManagerClient::initialize() {
    //packet_streamer
    packet_streamer_.setPacketFactory(&packet_factory_);

    int thread_count = TBSYS_CONFIG.getInt(
        "server","from_client_work_thread_count",4);
    task_queue_thread_.setThreadParameter(thread_count,this,NULL);
    return EXIT_SUCCESS;
}

int ManagerClient::destroy() {
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
    bp->set_direction(IO_DIRECTION_RECEIVE);
    task_queue_thread_.push(bp);

    return tbnet::IPacketHandler::KEEP_CHANNEL;
}

bool ManagerClient::handlePacketQueue(
tbnet::Packet * apacket, void *args) {
    BasePacket *packet = (BasePacket *) apacket;
    tbnet::Connection* conn=packet->get_connection();
    int64_t now_time=tbsys::CTimeUtil::getTime();

    if (now_time - packet->get_recv_time() > 
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
            RequestPacket *req = (RequestPacket *)packet;

            TBSYS_LOG(DEBUG,"client_req:chanid=%u|pcode=%u|msg_id=%lu|"
                "version=%u|append_args=%llu|src_type=%u|"
                "src_id=%llu|dest_type=%u|dest_id=%llu|data=%s",
                req->getChannelId(),req->getPCode(),
                req->msg_id_,req->version_,
                req->append_args_,req->src_type_,
                req->src_id_,req->dest_type_,
                req->dest_id_,req->data_);

            //检查注册状态
            ResponsePacket* resp=new ResponsePacket();
            if (NULL==resp) {
                return true;
            }
            resp->setChannelId(req->getChannelId());
            resp->src_type_=req->dest_type_;
            resp->src_id_=req->dest_id_;
            resp->dest_type_=req->src_type_;
            resp->dest_id_=req->src_id_;
            resp->msg_id_=req->msg_id_+1;
            resp->append_args_=req->append_args_;
            resp->error_code_=0;
            
            //普通业务处理
            resp->set_recv_time(now_time);
            Handler handler=HANDLER_ROUTER.GetHandler(
                req->msg_id_);
            if (NULL==handler) {
                resp->error_code_=4;
            } else {
                void args=0;
                handler(req,args,resp);
            }

            TBSYS_LOG(DEBUG,"server_resp:chanid=%u|pcode=%u|msg_id=%u|"
                "append_args=%llu|error_code=%u|src_type=%u|"
                "src_id=%llu|dest_type=%u|dest_id=%llu|data=%s",
                resp->getChannelId(),resp->getPCode(),
                resp->msg_id_,resp->append_args_,
                resp->error_code_,resp->src_type_,
                resp->src_id_,resp->dest_type_,
                resp->dest_id_,resp->data_);

            if (SERVER_TYPE_ACCESS_SERVER == resp->dest_type_) {
                //接入发来的请求，通过接入专道发给接入
                conn->getIOComponent()->subRef();
                if (!conn_manager_to_access_.postPacket(resp->dest_id_,resp)) {
                    delete resp;
                }
            } else {
                if (!conn->postPacket(resp)) {
                    delete resp;
                }
            }

            return true;
        }
        break;

        case RESPONSE_PACKET: {
            TBSYS_LOG(DEBUG,"%s","RESPONSE_PACKET");
        }
        break;
    }
    return true;
}


}
}
