#include "tbsys.h"
#include "tbnet.h"
#include "hiredis.h"
#include "redis_client_manager.h"
#include "redis_client.h"

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

void test_set(RedisClientManager* manager,int count) {
    char cmd[200];
    int64_t start,end;
    start=tbsys::CTimeUtil::getTime();
    for (int i=0;i<count;i++) {
        sprintf(cmd,"set yjb_%d %s",i,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
        RedisClient* rc=manager->GetRedisClient();
        if (NULL==rc || NULL==rc->redis_context_) {
            printf("connect redis fail\n");
            continue;
        }
        redisReply* reply=(redisReply* )redisCommand(rc->redis_context_,cmd);
        if (NULL==reply) {
            printf("%s","redis_client set fail");
            manager->ReleaseRedisClient(rc,false);
            continue;
        }
        //printf("type=%d, str=%s\n", reply->type, reply->str);
        if( !(reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str,"OK")==0)) {
            printf("%s","redis_client ping fail");
            freeReplyObject(reply);
            manager->ReleaseRedisClient(rc,true);
            continue;
        }
    
        freeReplyObject(reply);
        manager->ReleaseRedisClient(rc,true);
    }
    end=tbsys::CTimeUtil::getTime();
    printf("time=%lld\n", end-start);
}

void test_setnx(RedisClientManager* manager) {
    RedisClient* rc=manager->GetRedisClient();
    if (NULL==rc || NULL==rc->redis_context_) {
        printf("connect redis fail\n");
        return;
    }
    char cmd[100];
    sprintf(cmd,"set test_setnx test_setnx");
    redisReply* reply=(redisReply* )redisCommand(rc->redis_context_,cmd);
    if (NULL==reply) {
        printf("%s fail\n",cmd);
        manager->ReleaseRedisClient(rc,false);
        return;
    }
    printf("%s:type=%d, str=%s\n", cmd, reply->type, reply->str);
    freeReplyObject(reply);

    sprintf(cmd,"setnx test_setnx test_setnx");
    reply=(redisReply* )redisCommand(rc->redis_context_,cmd);
    if (NULL==reply) {
        printf("%s fail\n",cmd);
        manager->ReleaseRedisClient(rc,false);
        return;
    }
    printf("%s:type=%d, str=%s\n", cmd, reply->type, reply->str);
    
    freeReplyObject(reply);
    manager->ReleaseRedisClient(rc,true);
}

void test_Rcommand(RedisClientManager* manager) {
    RedisClient* rc=manager->GetRedisClient();
    redisReply* reply;
    int cmd_ret=Rcommand(rc, "ping", reply);
    printf("cmd_ret=%d\n",cmd_ret);
    if (SUCCESS_ACTIVE != cmd_ret) {
        manager->ReleaseRedisClient(rc, cmd_ret);
    }
    manager->ReleaseRedisClient(rc,true);
}

void test_Rhsetnx(RedisClientManager* manager) {
    RedisClient* rc=manager->GetRedisClient();
    redisReply* reply;
    int cmd_ret=Rhsetnx(rc, "HSETNX yjb life happyfull", reply);
    printf("cmd_ret=%d\n",cmd_ret);
    if (SUCCESS_ACTIVE != cmd_ret) {
        manager->ReleaseRedisClient(rc, cmd_ret);
    }
    manager->ReleaseRedisClient(rc,true);
}

void test_Rhmset(RedisClientManager* manager) {
    RedisClient* rc=manager->GetRedisClient();
    redisReply* reply;
    int cmd_ret=Rhmset(rc, "HMSET yjb life happyfull age 27 sex man", reply);
    printf("cmd_ret=%d\n",cmd_ret);
    if (SUCCESS_ACTIVE != cmd_ret) {
        manager->ReleaseRedisClient(rc, cmd_ret);
    }
    manager->ReleaseRedisClient(rc,true);
}

void test_Rhget(RedisClientManager* manager) {
    RedisClient* rc=manager->GetRedisClient();
    redisReply* reply;
    int cmd_ret=Rhget(rc, "HGET lljzfly life", reply);
    printf("cmd_ret=%d\n",cmd_ret);
    if (SUCCESS_ACTIVE != cmd_ret) {
        manager->ReleaseRedisClient(rc, cmd_ret);
    }
    manager->ReleaseRedisClient(rc,true);

    cmd_ret=Rhget(rc, "HGET lljzfly001 life", reply);
    printf("cmd_ret=%d\n",cmd_ret);
    if (SUCCESS_ACTIVE != cmd_ret) {
        manager->ReleaseRedisClient(rc, cmd_ret);
    }
    manager->ReleaseRedisClient(rc,true);
}

void test_Rsunionstore(RedisClientManager* manager) {
    RedisClient* rc=manager->GetRedisClient();
    int64_t start,end;
    redisReply* reply;
    char cmd[512];
    int cmd_ret;
    start=tbsys::CTimeUtil::getTime();
    int i;
    for (i=0;i<2;i++) {
        sprintf(cmd,"SADD global_file_id_sets %d",i);
        cmd_ret=Rsadd(rc, cmd, reply);
        if (SUCCESS_ACTIVE != cmd_ret) {
            manager->ReleaseRedisClient(rc, cmd_ret);
            printf("SADD fail\n");
            return;
        }
    }
    end=tbsys::CTimeUtil::getTime();
    printf("SADD %d records,time=%lld\n",i,end-start);

    sprintf(cmd,"%s","SUNIONSTORE file_id_sets global_file_id_sets");
    start=tbsys::CTimeUtil::getTime();
    cmd_ret=Rsunionstore(rc, cmd, reply);
    printf("cmd_ret=%d\n",cmd_ret);
    if (SUCCESS_ACTIVE != cmd_ret) {
        manager->ReleaseRedisClient(rc, cmd_ret);
        return;
    }
    end=tbsys::CTimeUtil::getTime();
    printf("SUNIONSTORE,time=%lld\n",end-start);
    manager->ReleaseRedisClient(rc,true);
}

int main(int argc,char* argv[]) {
    if(TBSYS_CONFIG.load("test.ini")) {
        printf("load test.ini fail");
        return -1;
    }

    const char* sz_log_level = TBSYS_CONFIG.getString(
    "public", "log_level", "info");
    printf("sz_log_level=%s\n", sz_log_level);
    TBSYS_LOGGER.setLogLevel(sz_log_level);

    RedisClientManager redis_client_manager;
    if (!redis_client_manager.start("redis")) {
        printf("start redis_client_manager fail\n");
        return -1;
    }
    sleep(3);

//    test_ping(&redis_client_manager);
//    test_set(&redis_client_manager, 10000);
//    test_set_one_client(&redis_client_manager, 1000);
//    test_setnx(&redis_client_manager);
//    test_Rcommand(&redis_client_manager);
//    test_Rhsetnx(&redis_client_manager);
//    test_Rhsetnx(&redis_client_manager);
//    test_Rhmset(&redis_client_manager);
//    test_Rhget(&redis_client_manager);
//    test_Rsunionstore(&redis_client_manager);

    redis_client_manager.stop();
    redis_client_manager.wait();
    return 0;
}
