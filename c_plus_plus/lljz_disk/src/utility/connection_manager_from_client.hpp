#ifndef LLJZ_DISK_CONNECTION_MANAGER_FROM_CLIENT_H
#define LLJZ_DISK_CONNECTION_MANAGER_FROM_CLIENT_H

#include <ext/hash_map>
#include "tbsys.h"
#include "tbnet.h"

namespace lljz {
namespace disk {

struct ConnectionInfo {
    tbnet::Connection* connection_;
    bool state_;
    atomic_t packetCount_; //收到Packet计数

    ConnectionInfo() {
        connection_=NULL;
        state_=false;
        atomic_set(&packetCount_,0);
    }

    int addCount() {
        return atomic_add_return(1,&packetCount_);
    }

    void subCount() {
        atomic_dec(&packetCount_);
    }

    int getCount() {
        return atomic_read(&packetCount_);
    }
};

class ConnectionManagerFromClient {
public:
    ConnectionManagerFromClient();
    ~ConnectionManagerFromClient();

    //注册来自客户端的连接
    //用于推送
    bool RegisterConnection(tbnet::Connection* connection);
    
    // 返回值无用
    bool PostPacket(tbnet::Packet* packet);


public:
    //
    __gnu_cxx::hash_map<tbnet::Connection*,ConnectionInfo*> connectionMap_;
    tbsys::CThreadMutex connMapMutex_;

    //chid到packet，暂存请求packet，内存开销
    //给chid超时时间，实时释放请求packet，可能有冲突chid
    //暂时牺牲空间,Connection*从pcket取
    __gnu_cxx::hash_map<uint32_t,tbnet::Connection*> chidMap_;
    tbsys::CThreadMutex chidMapMutex_;
};


}
}

#endif