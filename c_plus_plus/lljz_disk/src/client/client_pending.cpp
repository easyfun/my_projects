/*
 * (C) 2007-2010 Taobao Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id$
 *
 * Authors:
 *   duolong <duolong@taobao.com>
 *
 */

#include "tbnet.h"
#include "tbsys.h"
//#include "json/json.h"
#include "base_packet.hpp"
#include "request_packet.hpp"
#include "response_packet.hpp"
#include "packet_factory.hpp"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

using namespace lljz::disk;
using namespace tbnet;
using namespace rapidjson;

int gsendcount = 1000;
int64_t gsendlen = 0;
Transport transport;
atomic_t success_count;
atomic_t fail_count;

struct ConnectCond {
    tbnet::Connection* conn_;
    tbsys::CThreadCond cond_;
    int send_;
    RequestPacket* req_;
    ResponsePacket resp_;

    ConnectCond() {
        conn_=NULL;
        send_=0;
    }
};

class TestClientPacketHandler : public IPacketHandler
{
public:
    TestClientPacketHandler() {_recvlen = 0; _timeoutCount = 0; 
        atomic_set(&_count, 0);}
    HPRetCode handlePacket(Packet *packet, void *args)
    {
        RequestPacket *req;
        ResponsePacket *resp;
        ConnectCond* conn_cond;

        TBSYS_LOG(DEBUG,"handlePacket:gsendcount=%d,count=%d",gsendcount,_count.counter);
        if (!packet->isRegularPacket()) { // 是否正常的包
            tbnet::ControlPacket* ctrl_packet=(tbnet::ControlPacket* )packet;
            int cmd=ctrl_packet->getCommand();
            if (tbnet::ControlPacket::CMD_DISCONN_PACKET==cmd) {
                TBSYS_LOG(DEBUG,"%s",
                    "TestClientPacketHandler::handlePacket:DISCONN");
                return IPacketHandler::FREE_CHANNEL;
            } else if (tbnet::ControlPacket::CMD_TIMEOUT_PACKET==cmd) {
                _timeoutCount ++;
                atomic_inc(&_count);
                atomic_inc(&fail_count);
                TBSYS_LOG(DEBUG,"%s",
                    "TestClientPacketHandler::handlePacket:TIMEOUT");
                conn_cond=(ConnectCond* )args;
                conn_cond->cond_.lock();
                conn_cond->send_=2;
                conn_cond->cond_.signal();
                conn_cond->cond_.unlock();
            }

            return IPacketHandler::FREE_CHANNEL;
        }

        atomic_inc(&_count);
        conn_cond=(ConnectCond* )args;
        req=conn_cond->req_;
        resp = (ResponsePacket*)packet;
        if (0==resp->error_code_) {
            atomic_inc(&success_count);
            TBSYS_LOG(DEBUG,"resp:chanid=%u|pcode=%u|msg_id=%u|src_type=%u|"
                "src_id=%u|dest_type=%u|dest_id=%u|data=%s",
                resp->getChannelId(),resp->getPCode(),resp->msg_id_,
                resp->src_type_,resp->src_id_,resp->dest_type_,
                resp->dest_id_,resp->data_);
        } else {
            atomic_inc(&fail_count);
        }
        delete req;
        delete resp;

        conn_cond->cond_.lock();
        conn_cond->send_=1;
        conn_cond->cond_.signal();
        conn_cond->cond_.unlock();

        return IPacketHandler::FREE_CHANNEL;
    }    
private:
    atomic_t _count;
    int64_t _recvlen;
    int _timeoutCount;
};

class TestClient {
public:
    TestClient(char *spec);
    ~TestClient();
    void start(int c);
private:
    char *_spec;
};

TestClient::TestClient(char *spec)
{
    _spec = strdup(spec);
}

TestClient::~TestClient()
{
    if (_spec) {
        free(_spec);
    }
}

void TestClient::start(int conncount)
{
    PacketFactory factory;
    DefaultPacketStreamer streamer(&factory);
    TestClientPacketHandler handler;
    Connection **cons = (Connection**) malloc(conncount*sizeof(Connection*));
    
    int64_t startTime = tbsys::CTimeUtil::getTime();
    atomic_set(&success_count, 0);
    atomic_set(&fail_count, 0);

    for(int i=0; i<conncount; i++) {
        cons[i] = transport.connect(_spec, &streamer, true);
        if (cons[i] == NULL) {
            TBSYS_LOG(ERROR, "connection error.");
            return;
        }
        cons[i]->setDefaultPacketHandler(&handler);
        cons[i]->setQueueTimeout(180000);
        cons[i]->setQueueLimit(0);
    }
    transport.start();

    //rapidjson
    Document doc_req;
    Document::AllocatorType& allocator=doc_req.GetAllocator();
    Value json_req(kObjectType);
    Value json_spec(kStringType);
    json_spec="tcp:127.0.0.1:10800";

    Value json_srv_type(kNumberType);
    json_srv_type=(uint)SERVER_TYPE_CLIENT_LINUX;

    Value json_srv_id(kNumberType);
    json_srv_id=10000;

    Value json_dep_srv_type(kArrayType);
    int size=10;
    for (int i=0;i<size;i++) {
        json_dep_srv_type.PushBack(i,allocator);
    }
    json_req.AddMember("spec",json_spec,allocator);
    json_req.AddMember("srv_type",json_srv_type,allocator);
    json_req.AddMember("dep_srv_type",json_dep_srv_type,allocator);

    std::string req_data;
    int sendcount = 0;
    int pid = getpid();
    TBSYS_LOG(ERROR, "PID: %d", pid);
    for(int i=0; i<gsendcount; i++) {
        RequestPacket *packet = new RequestPacket();
        packet->src_type_=SERVER_TYPE_CLIENT_LINUX;
        packet->src_id_=10000+i;
        packet->dest_type_=SERVER_TYPE_CONFIG_SERVER;
        packet->dest_id_=0;
        packet->msg_id_=CONFIG_SERVER_GET_SERVICE_LIST_REQ;//CONFIG_SERVER_ECHO_TEST_REQ;

        uint64_t server_id;
        if (i<500) {
           server_id=10000+i;
        } else {
           server_id=10000+i%500;
        }
        json_req.RemoveMember("srv_id");
        json_srv_id=server_id;
        json_req.AddMember("srv_id",json_srv_id,allocator);

        StringBuffer req_buffer;
        Writer<StringBuffer> writer(req_buffer);
        json_req.Accept(writer);
        std::string req_data=req_buffer.GetString();
        strcat(packet->data_,req_data.c_str());

        sendcount++;
        ConnectCond conn_cond;
        conn_cond.cond_.lock();
        conn_cond.send_=0;
        conn_cond.conn_=cons[i%conncount];
        conn_cond.req_=packet;
        if (!cons[i%conncount]->postPacket(packet, NULL, &conn_cond)) {
            TBSYS_LOG(DEBUG,"%s","postPacket fail");
            break;
        }
        while (0==conn_cond.send_) {
            conn_cond.cond_.wait(1000);
        }
        TBSYS_LOG(DEBUG,"i=%d",i);
        conn_cond.cond_.unlock();
    }

    gsendcount = sendcount;
    TBSYS_LOG(ERROR, "send finish.");

    int64_t endTime = tbsys::CTimeUtil::getTime();

    TBSYS_LOG(ERROR, "speed: %d tps, agv size: %d, success_count: %d, fail_count=%d\n", 
        (int)((1000000LL * gsendcount)/(endTime-startTime)), 
        (int)(gsendlen/(gsendcount+1)), 
        success_count.counter,fail_count.counter);

    transport.stop();
    transport.wait();
    free(cons);
}

void singalHandler(int seg)
{
    transport.stop();
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        printf("%s [tcp|udp]:ip:port count conn\n", argv[0]);
        return EXIT_FAILURE;
    }
    int sendcount = atoi(argv[2]);
    if (sendcount > 0) {
        gsendcount = sendcount;
    }
    int conncount = atoi(argv[3]);
    if (conncount < 1) {
        conncount = 1;
    }
    signal(SIGINT, singalHandler);
    signal(SIGTERM, singalHandler);

    TBSYS_LOGGER.setLogLevel("ERROR");

    TestClient testServer(argv[1]);
    testServer.start(conncount);
     
    TBNET_GLOBAL_STAT.log();
    
    return EXIT_SUCCESS;
}


