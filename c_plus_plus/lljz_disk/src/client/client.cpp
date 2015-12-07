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
#include "json/json.h"
#include "base_packet.hpp"
#include "request_packet.hpp"
#include "response_packet.hpp"
#include "packet_factory.hpp"

using namespace lljz::disk;
using namespace tbnet;

int gsendcount = 1000;
int64_t gsendlen = 0;
Transport transport;
int encode_count = 0;

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
        //cons[i]->setQueueTimeout(3000);
        cons[i]->setQueueLimit(0);
    }
    transport.start();
//    char buffer[4096] = "{\"ip\":\"127.0.0.1\",\"mac\":\"010A0B0C\"}";
    Json::Value json_req;
    json_req["spec"]="tcp:127.0.0.1:10800";
    json_req["srv_type"]=SERVER_TYPE_CLIENT_LINUX;
    char int64_str[32]={0};
    sprintf(int64_str,"%llu",10000);
    json_req["srv_id"]=int64_str;
    Json::Value json_dep_srv_type=Json::Value(Json::arrayValue);
    Json::Value json_srv_type;
    int size=10;
    for (int i=0;i<size;i++) {
        json_srv_type=i;
        json_dep_srv_type.append(json_srv_type);
    }
    json_req["dep_srv_type"]=json_dep_srv_type;
    Json::FastWriter writer;
    std::string req_data;//=writer.write(json_req);

    int sendcount = 0;
    int pid = getpid();
    TBSYS_LOG(ERROR, "PID: %d", pid);
    for(int i=0; i<gsendcount; i++) {
        RequestPacket *packet = new RequestPacket();
        TBSYS_LOG(DEBUG,"TestClient::start, addr=%u",packet);
        packet->src_type_=SERVER_TYPE_CLIENT_LINUX;
        packet->src_id_=10000+i;
        packet->dest_type_=SERVER_TYPE_CONFIG_SERVER;
        packet->dest_id_=0;
        packet->msg_id_=CONFIG_SERVER_GET_SERVICE_LIST_REQ;
//        sprintf(packet->data_,"%s",buffer);

        if (i<500) {
           sprintf(int64_str,"%llu",10000+i);
        } else {
           sprintf(int64_str,"%llu",10000+i%500);
        }
        json_req["srv_id"]=int64_str;
        req_data=writer.write(json_req);
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
    testServer.start(conncount);
    int64_t endTime = tbsys::CTimeUtil::getTime();

    TBSYS_LOG(ERROR, "speed: %d tps, agv size: %d, encode_count: %d\n", 
        (int)((1000000LL * gsendcount)/(endTime-startTime)), 
        (int)(gsendlen/(gsendcount+1)), 
        encode_count);
     
    TBNET_GLOBAL_STAT.log();
    
    return EXIT_SUCCESS;
}


