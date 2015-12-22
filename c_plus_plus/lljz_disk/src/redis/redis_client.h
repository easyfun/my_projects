#ifndef LLJZ_DISK_REDIS_CLIENT_H
#define LLJZ_DISK_REDIS_CLIENT_H

#include "tbsys.h"
#include "tbnet.h"
#include "hiredis.h"

namespace lljz {
namespace disk {

enum {
    FAILED_NOT_ACTIVE=0,
    SUCCESS_ACTIVE=1,
    FAILED_ACTIVE=2,
};

struct RedisClient {
    redisContext* redis_context_;
//    int status_;//
    int64_t last_used_time_;

    RedisClient() {
        redis_context_=NULL;
        last_used_time_=0;
    }
};

//redis_content==NULL,reply==NULL，需要重连；命令错误，成功，不用重连
//0错误重连,1成功不重连，2错误不重连
int Rcommand(RedisClient* rc, const char* cmd,
            redisReply*& reply, bool free_reply=true);
int Rhsetnx(RedisClient* rc, const char* cmd, 
            redisReply*& reply, bool free_reply=true);
int Rhmset(RedisClient* rc, const char* cmd, 
            redisReply*& reply, bool free_reply=true);
int Rexists(RedisClient* rc, const char* cmd, 
            redisReply*& reply, bool free_reply=true);
int Rhset(RedisClient* rc, const char* cmd, 
            redisReply*& reply, bool free_reply=true);
int Rhget(RedisClient* rc, const char* cmd, 
            redisReply*& reply, bool free_reply=true);

//集合sets
int Rsadd(RedisClient* rc, const char* cmd, 
            redisReply*& reply, bool free_reply=true);
int Rspop(RedisClient* rc, const char* cmd, 
            redisReply*& reply, bool free_reply=true);
int Rsunionstore(RedisClient* rc, const char* cmd, 
            redisReply*& reply, bool free_reply=true);

}
}

#endif