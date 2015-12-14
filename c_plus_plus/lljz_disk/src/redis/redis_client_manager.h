#ifndef LLJZ_DISK_REDIS_CLIENT_MANAGER_H
#define LLJZ_DISK_REDIS_CLIENT_MANAGER_H

#include "tbnet.h"
#include "tbsys.h"
#include "hiredis/hiredis.h"

namespace lljz {
namespace disk {

struct RedisClient {
    redisContext* redis_context_;
    int status_;//
    int64_t last_used_time_;

    RedisClient() {
        memset(this,0,sizeof(RedisClient));
    }
};

class RedisClientManager : tbsys::Runnable {
public:
    RedisClientManager();
    ~RedisClientManager();

    bool start();
    bool stop();
    bool wait();
    void destroy();

    //Runnable
    void run(tbsys::CThread* thread, void* arg);
    void CheckReconn();
    void CheckFree();

    RedisClient* GetRedisClient();
    void ReleaseRedisClient(RedisClient* rc, bool active);

private:
    bool stop_;
    tbsys::CThread timer_thread_;

    char redis_host_[200];
    int redis_port_;

    RedisClient* redis_client_pool_;
    int redis_client_num_;
    int connect_time_out_;

    std::queue<RedisClient*> free_redis_client_;
    tbsys::CThreadMutext free_mutext_;
//    tbsys::CThreadCond free_cond_;


    std::queue<RedisClient*> reconn_redis_client_;
    tbsys::CThreadMutext reconn_mutext_;
//    tbsys::CThreadCond reconn_cond_;

};

}
}

#endif