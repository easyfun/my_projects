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
        RequestPacket *req=(RequestPacket *)args;
        ResponsePacket *resp = (ResponsePacket*)packet;
        atomic_inc(&_count);
        if (!packet->isRegularPacket()) { // 是否正常的包
//            TBSYS_LOG(ERROR, "INDEX: %d => ControlPacket: %d", resp->getIndex(), ((ControlPacket*)packet)->getCommand());
            _timeoutCount ++;
            if (_count.counter == gsendcount) {
                transport.stop();
            }
            delete req;
            return IPacketHandler::FREE_CHANNEL;
        }
//        _recvlen += ((ResponsePacket*)packet)->getRecvLen();

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
        cons[i]->setQueueLimit(0);
    }
    transport.start();
    char buffer[4096] = "{\"ip\":\"127.0.0.1\",\"mac\":\"010A0B0C\"}";
    int sendcount = 0;
    int pid = getpid();
    TBSYS_LOG(ERROR, "PID: %d", pid);
    for(int i=0; i<gsendcount; i++) {
        RequestPacket *packet = new RequestPacket();
        packet->setPCode(CONFIG_SERVER_GET_SERVICE_LIST_REQ);
        packet->src_id_=1;
        packet->dest_id_=100;
        packet->version_=0;
        sprintf(packet->data_,"%s",buffer);
        if (!cons[i%conncount]->postPacket(packet, NULL, packet)) {
            break;
        }
//        gsendlen += len;
        sendcount ++;
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


