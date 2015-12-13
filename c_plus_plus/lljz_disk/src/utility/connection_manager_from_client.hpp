#ifndef LLJZ_DISK_CONNECTION_MANAGER_FROM_CLIENT_H
#define LLJZ_DISK_CONNECTION_MANAGER_FROM_CLIENT_H

#include <ext/hash_map>
#include "tbsys.h"
#include "tbnet.h"

namespace lljz {
namespace disk {

typedef __gnu_cxx::hash_map<uint64_t,
                tbnet::Connection*> ConnFromClientMap;
typedef __gnu_cxx::hash_map<uint16_t,
                uint16_t> SupportServerTypeMap;
class ConnectionManagerFromClient:public tbsys::Runnable {
public:
    ConnectionManagerFromClient();
    ~ConnectionManagerFromClient();

    bool start();
    bool stop();
    bool wait();
    void destroy();

    //Runnable
    void run(tbsys::CThread* thread, void* arg);
    void CheckResend();

    //注册来自客户端的连接
    //用于推送
/*    
    int RegisterAccess(uint16_t type,uint64_t id, 
        tbnet::Connection* conn);
    bool IsRegisterAccess(uint64_t id);
*/
    int Register(uint64_t id,
        tbnet::Connection* conn);
    bool IsRegister(uint64_t id);
    bool IsSupportServerType(uint16_t type);

    void PostPacket(uint64_t id, tbnet::Packet* packet);



private:
    //
    ConnFromClientMap conn_from_client_;
    tbsys::CRWSimpleLock rw_lock_;

    //重发队列
    tbnet::PacketQueue resend_queue_;
    tbsys::CThreadMutex mutex_;   

    bool stop_;
    tbsys::CThread timer_thread_;

    SupportServerTypeMap support_server_type_;
};


}
}

#endif