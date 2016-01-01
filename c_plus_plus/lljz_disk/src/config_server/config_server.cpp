#include "config_server.hpp"
#include "request_packet.hpp"
#include "response_packet.hpp"
//#include "json/json.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

//using namespace tbnet;
using namespace rapidjson;

namespace lljz {
namespace disk {

ConfigServer::ConfigServer() {
    disconnThrowPackets_=0;
    timeoutThrowPackets_=0;
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
        int port=TBSYS_CONFIG.getInt("server","port",10000);
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

tbnet::IPacketHandler::HPRetCode ConfigServer::handlePacket(
tbnet::Connection *connection, tbnet::Packet *packet) {
    TBSYS_LOG(DEBUG,"%s","ConfigServer::handlePacket");
    if (!packet->isRegularPacket()) {
        TBSYS_LOG(ERROR,"ControlPacket, cmd: %d",
            ((tbnet::ControlPacket* )packet)->getCommand());

        return tbnet::IPacketHandler::FREE_CHANNEL;
    }

    RequestPacket *req = (RequestPacket *) packet;
    TBSYS_LOG(DEBUG,"req :chanid=%u|pcode=%u|msg_id=%u|src_type=%u|"
        "src_id=%u|dest_type=%u|dest_id=%u|data=%s",
        req->getChannelId(),req->getPCode(),req->msg_id_,
        req->src_type_,req->src_id_,req->dest_type_,
        req->dest_id_,req->data_);

    BasePacket* bp=(BasePacket* )packet;
    bp->set_recv_time(tbsys::CTimeUtil::getTime());
    bp->set_connection(connection);
    bp->set_direction(DIRECTION_RECEIVE);
    task_queue_thread_.push(bp);

    return tbnet::IPacketHandler::KEEP_CHANNEL;
}

bool ConfigServer::handlePacketQueue(tbnet::Packet * apacket, void *args) {
    TBSYS_LOG(DEBUG,"%s","ConfigServer::handlePacketQueue");
    BasePacket *packet = (BasePacket *) apacket;
    tbnet::Connection* conn=packet->get_connection();
    if (conn==NULL || conn->isConnectState()==false) {
        //避免失效连接上的业务处理消耗大量的时间
        disconnThrowPackets_++;
        TBSYS_LOG(DEBUG,"disconnThrowPackets_=%d",disconnThrowPackets_);
        return true;
    }
    int64_t nowTime=tbsys::CTimeUtil::getTime();
    if (nowTime-packet->get_recv_time() > PACKET_WAIT_FOR_SERVER_MAX_TIME) {
        //排队3min的请求丢弃
        timeoutThrowPackets_++;
        TBSYS_LOG(DEBUG,"timeoutThrowPackets_=%d",timeoutThrowPackets_);
        return true;
    }

    int pcode = packet->getPCode();
    switch (pcode) {
        case REQUEST_PACKET: {
            TBSYS_LOG(DEBUG,"%s","--------begin");
            RequestPacket *req = (RequestPacket *) packet;
            if (CONFIG_SERVER_GET_SERVICE_LIST_REQ==req->msg_id_) {
                /*
                T500:7000tps
                */
                TBSYS_LOG(DEBUG,"%s","--------begin-2");
                ResponsePacket *resp = new ResponsePacket();
                if (NULL==resp) {
                    TBSYS_LOG(DEBUG,"%s","memmory is not enough");
                    return true;
                }
                resp->setChannelId(req->getChannelId());
                resp->msg_id_=CONFIG_SERVER_GET_SERVICE_LIST_RESP;
                resp->src_type_=SERVER_TYPE_CONFIG_SERVER;
                resp->src_id_=req->dest_id_;
                resp->dest_type_=req->src_type_;
                resp->dest_id_=req->src_id_;
                resp->error_code_=0;
                /*
                //40000tps
                if(conn->postPacket(resp) == false) {
                    delete resp;
                }
                return true;*/

                //rapidjson
                Document document;
                document.Parse(req->data_);

                if (!document.HasMember("srv_id")||
                    !document["srv_id"].IsUint64() ||
                    0==document["srv_id"].GetUint64()) {
                    TBSYS_LOG(DEBUG,"%s","srv_id,error_code 10001");
                    resp->error_code_=10001;
                    if(conn->postPacket(resp) == false) {
                        delete resp;
                    }
                    return true;
                }
                uint64_t srv_id=document["srv_id"].GetUint64();
                TBSYS_LOG(DEBUG,"srv_id=%llu",srv_id);

                if (!document.HasMember("srv_type")||
                    !document["srv_type"].IsUint()) {
                    TBSYS_LOG(DEBUG,"%s","srv_type,error_code 10001");
                    resp->error_code_=10001;
                    if(conn->postPacket(resp) == false) {
                        delete resp;
                    }
                    return true;
                }
                uint16_t srv_type=document["srv_type"].GetUint();

                if (!document.HasMember("spec")||
                    !document["spec"].IsString()||
                    0==document["spec"].GetStringLength()) {
                    TBSYS_LOG(DEBUG,"%s","spec,error_code 10002");
                    resp->error_code_=10002;
                    if(conn->postPacket(resp) == false) {
                        delete resp;
                    }
                    return true;
                }
                std::string spec=document["spec"].GetString();

                if (!document.HasMember("dep_srv_type")||
                    !document["dep_srv_type"].IsArray()) {
                    TBSYS_LOG(DEBUG,"%s","dep_srv_type,error_code 10002");
                    resp->error_code_=10002;
                    if(conn->postPacket(resp) == false) {
                        delete resp;
                    }
                    return true;
                }
                const Value& json_dep_srv_type=document["dep_srv_type"];

                SrvURLInfoMap::iterator it;
                ServerURLInfo* srv_url_info;
                mutex_.lock();
                it=server_url_.find(srv_id);
                if (server_url_.end()==it) {
                    srv_url_info=new ServerURLInfo();
                } else {
                    srv_url_info=it->second;
                }
                sprintf(srv_url_info->spec_,"%s",spec.c_str());
                srv_url_info->server_type_=srv_type;
                srv_url_info->server_id_=srv_id;
                srv_url_info->last_use_time_=tbsys::CTimeUtil::getTime();
                server_url_[srv_id]=srv_url_info;

                int size=json_dep_srv_type.Size();
                int total=0;
                Document resp_doc;
                Document::AllocatorType& allocator=resp_doc.GetAllocator();
                Value resp_json(kObjectType);
                Value resp_json_total(kNumberType);
                Value resp_json_srv_info(kArrayType);

                for (it=server_url_.begin();server_url_.end()!=it;it++) {
                    TBSYS_LOG(DEBUG,"ServerURLInfo:spec_=%s,server_type_=%u,server_id_=%u",
                        srv_url_info->spec_,
                        srv_url_info->server_type_,
                        srv_url_info->server_id_);
                    for (int i=0;i<size;i++) {
                        srv_url_info=it->second;
                        if (srv_url_info->server_type_ !=
                            json_dep_srv_type[i].GetUint()) {
                            continue;
                        }
                        Value json_srv_url(kObjectType);
                        Value json_spec(kStringType);
                        Value json_srv_type(kNumberType);
                        Value json_srv_id(kNumberType);
                        json_spec.SetString(StringRef(srv_url_info->spec_));
                        json_srv_type=srv_url_info->server_type_;
                        json_srv_id=srv_url_info->server_id_;
                        json_srv_url.AddMember("spec",json_spec,allocator);
                        json_srv_url.AddMember("srv_type",json_srv_type,allocator);
                        json_srv_url.AddMember("srv_id",json_srv_id,allocator);
                        resp_json_srv_info.PushBack(json_srv_url,allocator);
                        total++;
                    }
                }
                mutex_.unlock();

                resp_json_total=total;
                resp_json.AddMember("total",resp_json_total,allocator);
                resp_json.AddMember("srv_info",resp_json_srv_info, allocator);

                StringBuffer resp_buffer;
                Writer<StringBuffer> writer(resp_buffer);
                resp_json.Accept(writer);
                std::string resp_data=resp_buffer.GetString();
                strcat(resp->data_,resp_data.c_str());
                TBSYS_LOG(DEBUG,"resp:chanid=%u|pcode=%u|msg_id=%u|src_type=%u|"
                    "src_id=%u|dest_type=%u|dest_id=%u|data=%s",
                    resp->getChannelId(),resp->getPCode(),resp->msg_id_,
                    resp->src_type_,resp->src_id_,resp->dest_type_,
                    resp->dest_id_,resp->data_);
                
                if(conn->postPacket(resp) == false) {
                    delete resp;
                }
            } else if (PUBLIC_ECHO_TEST_REQ==req->msg_id_) {
                ResponsePacket *resp = new ResponsePacket();
                if (NULL==resp) {
                    TBSYS_LOG(DEBUG,"%s","memmory is not enough");
                    return true;
                }
                mutex_.lock();
                resp->setChannelId(req->getChannelId());
                resp->msg_id_=req->msg_id_+1;
                resp->src_type_=SERVER_TYPE_CONFIG_SERVER;
                resp->src_id_=req->dest_id_;
                resp->dest_type_=req->src_type_;
                resp->dest_id_=req->src_id_;
                resp->error_code_=0;
                strcat(resp->data_,req->data_);

                TBSYS_LOG(DEBUG,"resp:chanid=%u|pcode=%u|msg_id=%u|src_type=%u|"
                    "src_id=%u|dest_type=%u|dest_id=%u|data=%s",
                    resp->getChannelId(),resp->getPCode(),resp->msg_id_,
                    resp->src_type_,resp->src_id_,resp->dest_type_,
                    resp->dest_id_,resp->data_);
                
                if(conn->postPacket(resp) == false) {
                    delete resp;
                }
                mutex_.unlock();
            } else {

            }

        }
        break;

        default: {

        }
        break;
    }
    return true;
}

int ConfigServer::Initialize() {
    SrvURLInfoMap::iterator it;
    mutex_.lock();
    it=server_url_.begin();
    for (;it!=server_url_.end();) {
        delete it->second;
        server_url_.erase(it);
        it=server_url_.begin();
    }
    mutex_.unlock();

    //packet_streamer
    packet_streamer_.setPacketFactory(&packet_factory_);

    int thread_count = TBSYS_CONFIG.getInt(
        "server","from_client_work_thread_count",4);
    task_queue_thread_.setThreadParameter(thread_count,this,NULL);
    return EXIT_SUCCESS;
}

int ConfigServer::Destroy() {
    TBSYS_LOG(ERROR,"ConfigServer::Destroy:size=%d",server_url_.size());

    SrvURLInfoMap::iterator it;
    mutex_.lock();
    it=server_url_.begin();
    for (;it!=server_url_.end();) {
        delete it->second;
        server_url_.erase(it);
        it=server_url_.begin();
    }
    mutex_.unlock();
    return EXIT_SUCCESS;
}

}
}
