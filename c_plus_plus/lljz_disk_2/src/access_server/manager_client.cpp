/*
#include "manager_client.h"
#include "request_packet.hpp"
#include "response_packet.hpp"
*/
#include "shared.h"

using namespace tbnet;

namespace lljz {
namespace disk {

ManagerClient::ManagerClient() {
    client_disconn_throw_packets_=0;
    queue_thread_timeout_throw_packets_=0;
    stop_=false;
}

ManagerClient::~ManagerClient() {
}

bool ManagerClient::start() {
    if (initialize()) {
        TBSYS_LOG(ERROR,"Initialize fail");
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
        timer_thread_.start(this,NULL);
    } else {
        return false;
    }

    return true;
}

bool ManagerClient::stop() {
    stop_=true;
    task_queue_thread_.stop();
    transport_for_client_.stop();
    return true;
}

bool ManagerClient::wait() {
    task_queue_thread_.wait();
    transport_for_client_.wait();
    timer_thread_.join();
    destroy();
    return true;
}

int ManagerClient::initialize() {
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

void ManagerClient::destroy() {
}

void ManagerClient::run(
tbsys::CThread* thread, void* arg) {
    Channel *list = NULL;
    Channel *channel = NULL;
    RequestInfo* req_info = NULL;
    int64_t now = 0;

    while (!stop_) {
        now = tbsys::CTimeUtil::getTime();
        list = channel_pool_->getTimeoutList(now);
        if (list != NULL) {
            channel = list;
            while (channel != NULL) {//超时，丢弃，不通知客户端
                req_info = (RequestInfo* )channel->getArgs();
                if (req_info) {
                    TBSYS_LOG(DEBUG,"id=%lu",req_info->chid_);
                    delete req_info;
                }
                channel->setArgs(NULL);
                channel = channel->getNext();
            }
            // 加到freelist中
            channel_pool_->appendFreeList(list);
        }

        usleep(500000);//500ms
    }
}

tbnet::IPacketHandler::HPRetCode 
ManagerClient::handlePacket(
tbnet::Connection *connection, 
tbnet::Packet *packet) {
    if (!packet->isRegularPacket()) {
        TBSYS_LOG(ERROR,"ControlPacket, cmd: %d",
            ((tbnet::ControlPacket* )packet)->getCommand());

        return tbnet::IPacketHandler::FREE_CHANNEL;
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

            TBSYS_LOG(DEBUG,"client_req:chanid=%u|pcode=%u|msg_id=%lu|"
                "version=%u|append_args=%llu|src_type=%u|"
                "src_id=%llu|dest_type=%u|dest_id=%llu|data=%s",
                client_req->getChannelId(),client_req->getPCode(),
                client_req->msg_id_,client_req->version_,
                client_req->append_args_,client_req->src_type_,
                client_req->src_id_,client_req->dest_type_,
                client_req->dest_id_,client_req->data_);

            //发送到业务服务器
            ResponsePacket* resp;
            RequestPacket* access_req=new RequestPacket();
            if (NULL==access_req) {
                resp=new ResponsePacket();
                SetErrorMsg(5,"can not reach to dest_server,RequestPacket,memory is used out",resp);
                if (!conn->postPacket(resp)) {
                    delete resp;
                }
                return true;
            }
            access_req->src_type_=SERVER_TYPE_ACCESS_SERVER;
            access_req->src_id_=self_server_id_;
            access_req->dest_type_=client_req->dest_type_;
            access_req->dest_id_=client_req->dest_id_;
            access_req->msg_id_=client_req->msg_id_;
            access_req->version_=client_req->version_;
            strcat(access_req->data_,client_req->data_);
            //分配请求id信息
            Channel *channel = NULL;
            channel = channel_pool_->allocChannel();
            // channel没找到了
            if (channel == NULL) {
                TBSYS_LOG(WARN, "分配channel出错");
                resp=new ResponsePacket();
                SetErrorMsg(5,"can not reach to dest_server,Channel,memory is used out",resp);
                if (!conn->postPacket(resp)) {
                    delete resp;
                }
                return true;
            }
            RequestInfo* req_info=new RequestInfo;
            req_info->chid_=client_req->getChannelId();
            req_info->src_type_=client_req->src_type_;
            req_info->src_id_=client_req->src_id_;
            req_info->conn_=conn;
            req_info->append_args_=client_req->append_args_;
            req_info->expire_time_=now_time + static_cast<int64_t>(10000000);
            uint32_t id=channel->getId();
            channel->setArgs(req_info);
            channel_pool_->setExpireTime(channel,req_info->expire_time_);

            access_req->append_args_=id;

            TBSYS_LOG(DEBUG,"access_req:chanid=%u|pcode=%u|msg_id=%lu"
                "version=%u|append_args=%llu|src_type=%u|"
                "src_id=%llu|dest_type=%u|dest_id=%llu|data=%s",
                access_req->getChannelId(),access_req->getPCode(),
                access_req->msg_id_,access_req->version_,
                access_req->append_args_,access_req->src_type_,
                access_req->src_id_,access_req->dest_type_,
                access_req->dest_id_,access_req->data_);

            if (!manager_server_->postPacket(access_req->dest_type_,access_req,NULL)) {
                //发送失败
                channel_pool_->freeChannel(channel);
                delete req_info;

                resp=new ResponsePacket();
                SetErrorMsg(5,"can not reach to dest_server,network error",resp);
                if (!conn->postPacket(resp)) {
                    delete resp;
                }
                return true;
            }

            return true;
        }
        break;

        case RESPONSE_PACKET: {
            //发送到客户端
            ResponsePacket *server_resp = (ResponsePacket *)packet;

            TBSYS_LOG(DEBUG,"server_resp:chanid=%lu|pcode=%u|msg_id=%lu|"
                "append_args=%llu|error_code=%u|src_type=%u|"
                "src_id=%llu|dest_type=%u|dest_id=%llu|data=%s",
                server_resp->getChannelId(),server_resp->getPCode(),
                server_resp->msg_id_,server_resp->append_args_,
                server_resp->error_code_,server_resp->src_type_,
                server_resp->src_id_,server_resp->dest_type_,
                server_resp->dest_id_,server_resp->data_);

            if (self_server_id_ == server_resp->dest_id_) {
                conn->getIOComponent()->subRef();
            }

            uint32_t chanid=(uint32_t)server_resp->append_args_;
            tbnet::Channel* channel=channel_pool_->offerChannel(chanid);
            if (NULL==channel) {
                return true;
            }
            RequestInfo* req_info=(RequestInfo* )channel->getArgs();
            channel_pool_->appendFreeList(channel);
            if (NULL==req_info) {
                return true;
            }

            ResponsePacket* resp=new ResponsePacket();
            resp->setChannelId(req_info->chid_);
            resp->src_type_=server_resp->src_type_;
            resp->src_id_=server_resp->src_id_;
            resp->dest_type_=req_info->src_type_;
            resp->dest_id_=req_info->src_id_;
            resp->msg_id_=server_resp->msg_id_;
            resp->append_args_=req_info->append_args_;
            resp->error_code_=server_resp->error_code_;
            strcat(resp->data_,server_resp->data_);

            TBSYS_LOG(DEBUG,"server_resp:chanid=%u|pcode=%u|msg_id=%lu|"
                "append_args=%llu|error_code=%u|src_type=%u|"
                "src_id=%llu|dest_type=%u|dest_id=%u|data=%s",
                resp->getChannelId(),resp->getPCode(),
                resp->msg_id_,resp->append_args_,
                resp->error_code_,resp->src_type_,
                resp->src_id_,resp->dest_type_,
                resp->dest_id_,resp->data_);

            tbnet::Connection* client_conn=req_info->conn_;
            delete req_info;

            if (NULL==client_conn) {
                return true;
            }

            if (!client_conn->postPacket(resp)) {
                delete resp;
            }

            return true;
        }
        break;
    }
    return true;
}


}
}
