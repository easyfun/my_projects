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

class TestClientPacketHandler : public IPacketHandler
{
public:
    TestClientPacketHandler() {_recvlen = 0; _timeoutCount = 0; 
        atomic_set(&_count, 0);}
    HPRetCode handlePacket(Packet *packet, void *args)
    {
        RequestPacket *req;
        ResponsePacket *resp;

        TBSYS_LOG(DEBUG,"handlePacket:gsendcount=%d,count=%d",gsendcount,_count.counter);
        if (!packet->isRegularPacket()) { // 是否正常的包
            tbnet::ControlPacket* ctrl_packet=(tbnet::ControlPacket* )packet;
            int cmd=ctrl_packet->getCommand();
            if (tbnet::ControlPacket::CMD_DISCONN_PACKET==cmd) {
                TBSYS_LOG(DEBUG,"%s",
                    "TestClientPacketHandler::handlePacket:DISCONN");
                return IPacketHandler::FREE_CHANNEL;
            } else if (tbnet::ControlPacket::CMD_TIMEOUT_PACKET==cmd) {
                atomic_inc(&_count);
                atomic_inc(&fail_count);
                TBSYS_LOG(DEBUG,"%s",
                    "TestClientPacketHandler::handlePacket:TIMEOUT");
            }

//            TBSYS_LOG(ERROR, "INDEX: %d => ControlPacket: %d", resp->getIndex(), ((ControlPacket*)packet)->getCommand());
            _timeoutCount ++;
            if (_count.counter == gsendcount) {
                transport.stop();
            }
            return IPacketHandler::FREE_CHANNEL;
        }
//        _recvlen += ((ResponsePacket*)packet)->getRecvLen();

        atomic_inc(&_count);
        req=(RequestPacket *)args;
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
//        printf("src_id=%u,dest_id=%u,data=%s\n",resp->src_id_,resp->dest_id_,resp->data_);
        if (_count.counter == gsendcount) {
            TBSYS_LOG(INFO, "OK=>_count: %d _timeoutCount: %d", _count.counter, _timeoutCount);        
            transport.stop(); 
        } else {    
            TBSYS_LOG(INFO, "_count: %d _timeoutCount: %d", _count.counter, _timeoutCount);        
        }
        delete req;
        delete resp;
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
//    char buffer[4096] = "{\"ip\":\"127.0.0.1\",\"mac\":\"010A0B0C\"}";

    //rapidjson
    Document doc_req;
    Document::AllocatorType& allocator=doc_req.GetAllocator();
    Value json_req(kObjectType);
    Value json_spec(kStringType);
    json_spec="tcp:127.0.0.1:10800";


    Value json_srv_id(kNumberType);
    json_srv_id=10000;

/*    Value json_dep_srv_type(kArrayType);
    int size=10;
    for (int i=0;i<size;i++) {
        json_dep_srv_type.PushBack(i,allocator);
    }*/
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    json_req.AddMember("spec",json_spec,allocator);
    std::string req_data;
    int sendcount = 0;
    int pid = getpid();
    TBSYS_LOG(ERROR, "PID: %d", pid);
    for(int i=0; i<gsendcount; i++) {
        RequestPacket *packet = new RequestPacket();
        TBSYS_LOG(DEBUG,"TestClient::start, addr=%u",packet);
        packet->src_id_=10000+i;
        packet->dest_type_=SERVER_TYPE_CONFIG_SERVER;
        packet->dest_id_=0;
        packet->msg_id_=CONFIG_SERVER_GET_SERVICE_LIST_REQ;//CONFIG_SERVER_ECHO_TEST_REQ;
//        sprintf(packet->data_,"%s",buffer);

        if (i<25) {
            packet->src_type_=i;
        } else {
            packet->src_type_=i%25;
        }
        Value json_dep_srv_type(kArrayType);
        json_dep_srv_type.PushBack(packet->src_type_,allocator);
        json_req.RemoveMember("dep_srv_type");
        json_req.AddMember("dep_srv_type",json_dep_srv_type,allocator);

        Value json_srv_type(kNumberType);
        json_srv_type=packet->src_type_;
        json_req.RemoveMember("srv_type");
        json_req.AddMember("srv_type",json_srv_type,allocator);

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
        if (!cons[i%conncount]->postPacket(packet, NULL, packet)) {
            break;
        }
//        gsendlen += len;
    }

    gsendcount = sendcount;
    TBSYS_LOG(ERROR, "send finish.");
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

    int64_t startTime = tbsys::CTimeUtil::getTime();
    srand(time(NULL));
    TestClient testServer(argv[1]);

    atomic_set(&success_count, 0);
    atomic_set(&fail_count, 0);

    testServer.start(conncount);
    int64_t endTime = tbsys::CTimeUtil::getTime();

    TBSYS_LOG(ERROR, "speed: %d tps, agv size: %d, success_count: %d, fail_count=%d\n", 
        (int)((1000000LL * gsendcount)/(endTime-startTime)), 
        (int)(gsendlen/(gsendcount+1)), 
        success_count.counter,fail_count.counter);
     
    TBNET_GLOBAL_STAT.log();
    
    return EXIT_SUCCESS;
}


