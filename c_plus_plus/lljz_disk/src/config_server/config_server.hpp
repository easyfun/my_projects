/*
测试性能: 
两台普通PC,centos 64bit
Map 500条记录
1000连接，100000次请求
6000笔/秒

jsoncpp比rapidjson差至少3部的性能

[TODO]
1.注册新业务服务，推送
2.心跳检查，踢掉不可用的服务，推送
*/

#ifndef LLJZ_DISK_CONFIG_SERVER_H_
#define LLJZ_DISK_CONFIG_SERVER_H_

#include "tbsys.h"
#include "tbnet.h"
#include "packet_factory.hpp"
#include "global.h"

namespace lljz {
namespace disk {

struct ServerURLInfo {
    char spec_[100];
    uint32_t server_type_;
    uint64_t server_id_;
    int64_t last_use_time_;//有效期180s

    ServerURLInfo() {
        memset(this,0,sizeof(ServerURLInfo));
    }
};

typedef __gnu_cxx::hash_map<uint64_t,ServerURLInfo*> SrvURLInfoMap;

class ConfigServer : public tbnet::IServerAdapter,
    public tbnet::IPacketQueueHandler {
public:
    ConfigServer();
    ~ConfigServer();

    void Start();
    void Stop();

    //IServerAdapter interface
    virtual tbnet::IPacketHandler::HPRetCode 
    handlePacket(tbnet::Connection *connection, tbnet::Packet *packet);

    // IPacketQueueHandler interface
    bool handlePacketQueue(tbnet::Packet * apacket, void *args);

private:
    inline int Initialize();
    inline int Destroy();

private:
    PacketFactory packet_factory_;
    tbnet::DefaultPacketStreamer packet_streamer_;
    tbnet::Transport packet_transport_;
//    tbnet::Transport heartbeat_transport_;

    tbnet::PacketQueueThread task_queue_thread_;
//    server_conf_thread my_server_conf_thread_;
    int64_t disconnThrowPackets_;   // 连接失效丢弃的请求包数
    int64_t timeoutThrowPackets_;   // 排队超时丢弃的请求包数

    SrvURLInfoMap server_url_;
    tbsys::CThreadMutex mutex_;
};

}
}

#endif
