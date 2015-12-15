#include "tbsys.h"
#include "tbnet.h"
#include "redis_client_manager.h"
#include "hiredis.h"
using namespace lljz::disk;

void test_ping(RedisClientManager* manager) {
    RedisClient* rc=manager->GetRedisClient();
    if (NULL==rc || NULL==rc->redis_context_) {
        printf("connect redis fail\n");
        return;
    }
    redisReply* reply=(redisReply* )redisCommand(rc->redis_context_,"ping");
    if (NULL==reply) {
        printf("%s","redis_client ping fail");
        manager->ReleaseRedisClient(rc,false);
        return;
    }
    printf("type=%d, str=%s\n", reply->type, reply->str);
    if( !(reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str,"pong")==0)) {
        printf("%s","redis_client ping fail");
        freeReplyObject(reply);
        manager->ReleaseRedisClient(rc,false);
        return;
    }
    
    freeReplyObject(reply);
    manager->ReleaseRedisClient(rc,true);
}

void test_set(RedisClientManager* manager,const char* cmd) {
    RedisClient* rc=manager->GetRedisClient();
    if (NULL==rc || NULL==rc->redis_context_) {
        printf("connect redis fail\n");
        return;
    }
    redisReply* reply=(redisReply* )redisCommand(rc->redis_context_,cmd);
    if (NULL==reply) {
        printf("%s","redis_client set fail");
        manager->ReleaseRedisClient(rc,false);
        return;
    }
    //printf("type=%d, str=%s\n", reply->type, reply->str);
    if( !(reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str,"OK")==0)) {
        printf("%s","redis_client ping fail");
        freeReplyObject(reply);
        manager->ReleaseRedisClient(rc,false);
        return;
    }
    
    freeReplyObject(reply);
    manager->ReleaseRedisClient(rc,true);
}

int main(int argc,char* argv[]) {
    if(TBSYS_CONFIG.load("test.ini")) {
        printf("load test.ini fail");
        return -1;
    }
    RedisClientManager redis_client_manager;
    redis_client_manager.start();
    sleep(3);

    test_ping(&redis_client_manager);
    char cmd[200];
    int64_t start=tbsys::CTimeUtil::getTime();
    for (int i=0;i<100000;i++) {
        sprintf(cmd,"set yjb_%d %d",i,1000+i);
        test_set(&redis_client_manager, cmd);
    }
    int64_t end=tbsys::CTimeUtil::getTime();
    printf("time=%lld\n", end-start);

    redis_client_manager.stop();
    redis_client_manager.wait();
    return 0;
}
