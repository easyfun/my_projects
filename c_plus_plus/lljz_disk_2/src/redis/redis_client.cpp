#include "redis_client.h"

namespace lljz {
namespace disk {

int Rcommand(RedisClient* rc, const char* cmd,
redisReply*& reply, bool free_reply) {
    if (NULL==rc) {
        TBSYS_LOG(DEBUG,"NULL==rc");
        return FAILED_NOT_ACTIVE;
    }

    if (NULL==rc->redis_context_) {
        TBSYS_LOG(DEBUG,"NULL==rc->redis_context_");
        return FAILED_NOT_ACTIVE;
    }

    reply=(redisReply* )redisCommand(rc->redis_context_,cmd);
    if (NULL==reply) {
        TBSYS_LOG(DEBUG,"NULL==reply");
        return FAILED_NOT_ACTIVE;
    }
    TBSYS_LOG(DEBUG,"type=%d,str=%s",reply->type, reply->str);

    int ret=FAILED_ACTIVE;
    if (REDIS_REPLY_STATUS==reply->type) {
        if (0==strcasecmp(cmd,"ping") && 0==strcasecmp(reply->str,"pong")) {
            ret=SUCCESS_ACTIVE;
        } else if (0==strcasecmp(cmd,"flushall") && 0==strcasecmp(reply->str,"ok")) {
            ret=SUCCESS_ACTIVE;
        }
        else {
            ret=FAILED_NOT_ACTIVE;
        }
    }

    if (free_reply) {
        freeReplyObject(reply);
    }

    TBSYS_LOG(DEBUG,"ret=%d",ret);
    return ret;
}

int Rexists(RedisClient* rc, const char* cmd, 
redisReply*& reply, bool free_reply) {
    if (NULL==rc) {
        TBSYS_LOG(DEBUG,"NULL==rc");
        return FAILED_NOT_ACTIVE;
    }

    if (NULL==rc->redis_context_) {
        TBSYS_LOG(DEBUG,"NULL==rc->redis_context_");
        return FAILED_NOT_ACTIVE;
    }

    TBSYS_LOG(DEBUG,"cmd=%s", cmd);
    if (0!=strncasecmp("EXISTS ",cmd,7)) {
        TBSYS_LOG(DEBUG,"not EXISTS");
        return FAILED_ACTIVE;
    }

    reply=(redisReply* )redisCommand(rc->redis_context_,cmd);
    if (NULL==reply) {
        TBSYS_LOG(DEBUG,"NULL==reply");
        return FAILED_NOT_ACTIVE;
    }
    TBSYS_LOG(DEBUG,"type=%d,integer=%d",reply->type, reply->integer);

    int ret=FAILED_ACTIVE;
    if (REDIS_REPLY_INTEGER==reply->type 
        && 1==reply->integer) {
        ret=SUCCESS_ACTIVE;
    }

    if (free_reply) {
        freeReplyObject(reply);
    }
    TBSYS_LOG(DEBUG,"ret=%d",ret);
    return ret;
}


/************************************************
//hash
************************************************/
int Rhsetnx(RedisClient* rc, const char* cmd, 
redisReply*& reply, bool free_reply) {
    if (NULL==rc) {
        TBSYS_LOG(DEBUG,"NULL==rc");
        return FAILED_NOT_ACTIVE;
    }

    if (NULL==rc->redis_context_) {
        TBSYS_LOG(DEBUG,"NULL==rc->redis_context_");
        return FAILED_NOT_ACTIVE;
    }

    TBSYS_LOG(DEBUG,"cmd=%s", cmd);
    if (0!=strncasecmp("HSETNX ",cmd,7)) {
        TBSYS_LOG(DEBUG,"not HSETNX");
        return FAILED_ACTIVE;
    }

    reply=(redisReply* )redisCommand(rc->redis_context_,cmd);
    if (NULL==reply) {
        TBSYS_LOG(DEBUG,"NULL==reply");
        return FAILED_NOT_ACTIVE;
    }

    TBSYS_LOG(DEBUG,"type=%d,integer=%d",reply->type, reply->integer);
    int ret=FAILED_ACTIVE;
    if (REDIS_REPLY_INTEGER==reply->type 
        && 1==reply->integer) {
        ret=SUCCESS_ACTIVE;
    }

    if (free_reply) {
        freeReplyObject(reply);
    }

    TBSYS_LOG(DEBUG,"ret=%d",ret);
    return ret;
}

int Rhmset(RedisClient* rc, const char* cmd, 
redisReply*& reply, bool free_reply) {
    if (NULL==rc) {
        TBSYS_LOG(DEBUG,"NULL==rc");
        return FAILED_NOT_ACTIVE;
    }

    if (NULL==rc->redis_context_) {
        TBSYS_LOG(DEBUG,"NULL==rc->redis_context_");
        return FAILED_NOT_ACTIVE;
    }

    TBSYS_LOG(DEBUG,"cmd=%s", cmd);
    if (0!=strncasecmp("HMSET ",cmd,6)) {
        TBSYS_LOG(DEBUG,"not HMSET");
        return FAILED_ACTIVE;
    }

    reply=(redisReply* )redisCommand(rc->redis_context_,cmd);
    if (NULL==reply) {
        TBSYS_LOG(DEBUG,"NULL==reply");
        return FAILED_NOT_ACTIVE;
    }
    TBSYS_LOG(DEBUG,"type=%d,str=%s",reply->type, reply->str);

    int ret=FAILED_ACTIVE;
    if (REDIS_REPLY_STATUS==reply->type 
        && 0==strcasecmp(reply->str,"OK")) {
        ret=SUCCESS_ACTIVE;
    }

    if (free_reply) {
        freeReplyObject(reply);
    }
    TBSYS_LOG(DEBUG,"ret=%d",ret);
    return ret;
}

int Rhset(RedisClient* rc, const char* cmd, 
redisReply*& reply, bool free_reply) {
    if (NULL==rc) {
        TBSYS_LOG(DEBUG,"NULL==rc");
        return FAILED_NOT_ACTIVE;
    }

    if (NULL==rc->redis_context_) {
        TBSYS_LOG(DEBUG,"NULL==rc->redis_context_");
        return FAILED_NOT_ACTIVE;
    }

    TBSYS_LOG(DEBUG,"cmd=%s", cmd);
    if (0!=strncasecmp("HSET ",cmd,5)) {
        TBSYS_LOG(DEBUG,"not HSET");
        return FAILED_ACTIVE;
    }

    reply=(redisReply* )redisCommand(rc->redis_context_,cmd);
    if (NULL==reply) {
        TBSYS_LOG(DEBUG,"NULL==reply");
        return FAILED_NOT_ACTIVE;
    }
    TBSYS_LOG(DEBUG,"type=%d,integer=%d",reply->type, reply->integer);

    int ret=FAILED_ACTIVE;
    if (REDIS_REPLY_INTEGER==reply->type) {
        ret=SUCCESS_ACTIVE;
    }

    if (free_reply) {
        freeReplyObject(reply);
    }
    TBSYS_LOG(DEBUG,"ret=%d",ret);
    return ret;
}

int Rhget(RedisClient* rc, const char* cmd, 
redisReply*& reply, bool free_reply) {
    TBSYS_LOG(DEBUG,"free_reply=%d",free_reply);
    if (NULL==rc) {
        TBSYS_LOG(DEBUG,"NULL==rc");
        return FAILED_NOT_ACTIVE;
    }

    if (NULL==rc->redis_context_) {
        TBSYS_LOG(DEBUG,"NULL==rc->redis_context_");
        return FAILED_NOT_ACTIVE;
    }

    TBSYS_LOG(DEBUG,"cmd=%s", cmd);
    if (0!=strncasecmp("HGET ",cmd,5)) {
        TBSYS_LOG(DEBUG,"not HGET");
        return FAILED_ACTIVE;
    }

    reply=(redisReply* )redisCommand(rc->redis_context_,cmd);
    if (NULL==reply) {
        TBSYS_LOG(DEBUG,"NULL==reply");
        return FAILED_NOT_ACTIVE;
    }
    TBSYS_LOG(DEBUG,"type=%d,str=%s",reply->type, reply->str);

    int ret=FAILED_ACTIVE;
    if (REDIS_REPLY_STRING==reply->type) {
        ret=SUCCESS_ACTIVE;
    }

    if (free_reply) {
        freeReplyObject(reply);
    }
    TBSYS_LOG(DEBUG,"ret=%d",ret);
    return ret;
}

int Rhdel(RedisClient* rc, const char* cmd, 
redisReply*& reply, bool free_reply) {
    if (NULL==rc) {
        TBSYS_LOG(DEBUG,"NULL==rc");
        return FAILED_NOT_ACTIVE;
    }

    if (NULL==rc->redis_context_) {
        TBSYS_LOG(DEBUG,"NULL==rc->redis_context_");
        return FAILED_NOT_ACTIVE;
    }

    TBSYS_LOG(DEBUG,"cmd=%s", cmd);
    if (0!=strncasecmp("HDEL ",cmd,5)) {
        TBSYS_LOG(DEBUG,"not HDEL");
        return FAILED_ACTIVE;
    }

    reply=(redisReply* )redisCommand(rc->redis_context_,cmd);
    if (NULL==reply) {
        TBSYS_LOG(DEBUG,"NULL==reply");
        return FAILED_NOT_ACTIVE;
    }
    TBSYS_LOG(DEBUG,"type=%d,integer=%d",reply->type, reply->integer);

    int ret=FAILED_ACTIVE;
    if (REDIS_REPLY_INTEGER==reply->type
        && 1==reply->integer) {
        ret=SUCCESS_ACTIVE;
    }

    if (free_reply) {
        freeReplyObject(reply);
    }
    TBSYS_LOG(DEBUG,"ret=%d",ret);
    return ret;
}

/**********************************************
// set
**********************************************/
int Rsadd(RedisClient* rc, const char* cmd, 
redisReply*& reply, bool free_reply) {
    if (NULL==rc) {
        TBSYS_LOG(DEBUG,"NULL==rc");
        return FAILED_NOT_ACTIVE;
    }

    if (NULL==rc->redis_context_) {
        TBSYS_LOG(DEBUG,"NULL==rc->redis_context_");
        return FAILED_NOT_ACTIVE;
    }

    TBSYS_LOG(DEBUG,"cmd=%s", cmd);
    if (0!=strncasecmp("SADD ",cmd,5)) {
        TBSYS_LOG(DEBUG,"not SADD");
        return FAILED_ACTIVE;
    }

    reply=(redisReply* )redisCommand(rc->redis_context_,cmd);
    if (NULL==reply) {
        TBSYS_LOG(DEBUG,"NULL==reply");
        return FAILED_NOT_ACTIVE;
    }
    TBSYS_LOG(DEBUG,"type=%d,integer=%d",reply->type, reply->integer);

    int ret=FAILED_ACTIVE;
    if (REDIS_REPLY_INTEGER==reply->type 
        && 1==reply->integer) {
        ret=SUCCESS_ACTIVE;
    }

    if (free_reply) {
        freeReplyObject(reply);
    }
    TBSYS_LOG(DEBUG,"ret=%d",ret);
    return ret;
}

int Rspop(RedisClient* rc, const char* cmd, 
redisReply*& reply, bool free_reply) {
    if (NULL==rc) {
        TBSYS_LOG(DEBUG,"NULL==rc");
        return FAILED_NOT_ACTIVE;
    }

    if (NULL==rc->redis_context_) {
        TBSYS_LOG(DEBUG,"NULL==rc->redis_context_");
        return FAILED_NOT_ACTIVE;
    }

    TBSYS_LOG(DEBUG,"cmd=%s", cmd);
    if (0!=strncasecmp("SPOP ",cmd,5)) {
        TBSYS_LOG(DEBUG,"not SPOP");
        return FAILED_ACTIVE;
    }

    reply=(redisReply* )redisCommand(rc->redis_context_,cmd);
    if (NULL==reply) {
        TBSYS_LOG(DEBUG,"NULL==reply");
        return FAILED_NOT_ACTIVE;
    }
    TBSYS_LOG(DEBUG,"type=%d,str=%d",reply->type, reply->str);

    int ret=FAILED_ACTIVE;
    if (REDIS_REPLY_STRING==reply->type) {
        ret=SUCCESS_ACTIVE;
    }

    if (free_reply) {
        freeReplyObject(reply);
    }
    TBSYS_LOG(DEBUG,"ret=%d",ret);
    return ret;
}


int Rsunionstore(RedisClient* rc, const char* cmd, 
redisReply*& reply, bool free_reply) {
    if (NULL==rc) {
        TBSYS_LOG(DEBUG,"NULL==rc");
        return FAILED_NOT_ACTIVE;
    }

    if (NULL==rc->redis_context_) {
        TBSYS_LOG(DEBUG,"NULL==rc->redis_context_");
        return FAILED_NOT_ACTIVE;
    }

    TBSYS_LOG(DEBUG,"cmd=%s", cmd);
    if (0!=strncasecmp("SUNIONSTORE ",cmd,12)) {
        TBSYS_LOG(DEBUG,"not SUNIONSTORE");
        return FAILED_ACTIVE;
    }

    reply=(redisReply* )redisCommand(rc->redis_context_,cmd);
    if (NULL==reply) {
        TBSYS_LOG(DEBUG,"NULL==reply");
        return FAILED_NOT_ACTIVE;
    }
    TBSYS_LOG(DEBUG,"type=%d,integer=%d",reply->type, reply->integer);

    int ret=FAILED_ACTIVE;
    if (REDIS_REPLY_INTEGER==reply->type && reply->integer>=1) {
        ret=SUCCESS_ACTIVE;
    }

    if (free_reply) {
        freeReplyObject(reply);
    }
    TBSYS_LOG(DEBUG,"ret=%d",ret);
    return ret;
}

/*********************************************
//有序集合zsets
*********************************************/
int Rzadd(RedisClient* rc, const char* cmd, 
redisReply*& reply, bool free_reply) {
    if (NULL==rc) {
        TBSYS_LOG(DEBUG,"NULL==rc");
        return FAILED_NOT_ACTIVE;
    }

    if (NULL==rc->redis_context_) {
        TBSYS_LOG(DEBUG,"NULL==rc->redis_context_");
        return FAILED_NOT_ACTIVE;
    }

    TBSYS_LOG(DEBUG,"cmd=%s", cmd);
    if (0!=strncasecmp("ZADD ",cmd,5)) {
        TBSYS_LOG(DEBUG,"not ZADD");
        return FAILED_ACTIVE;
    }

    reply=(redisReply* )redisCommand(rc->redis_context_,cmd);
    if (NULL==reply) {
        TBSYS_LOG(DEBUG,"NULL==reply");
        return FAILED_NOT_ACTIVE;
    }
    TBSYS_LOG(DEBUG,"type=%d,integer=%d",reply->type, reply->integer);

    int ret=FAILED_ACTIVE;
    if (REDIS_REPLY_INTEGER==reply->type) {
        ret=SUCCESS_ACTIVE;
    }

    if (free_reply || FAILED_ACTIVE==ret) {
        freeReplyObject(reply);
    }
    TBSYS_LOG(DEBUG,"ret=%d",ret);
    return ret;
}

int Rzrangebyscore(RedisClient* rc, const char* cmd, 
redisReply*& reply, bool free_reply) {
    if (NULL==rc) {
        TBSYS_LOG(DEBUG,"NULL==rc");
        return FAILED_NOT_ACTIVE;
    }

    if (NULL==rc->redis_context_) {
        TBSYS_LOG(DEBUG,"NULL==rc->redis_context_");
        return FAILED_NOT_ACTIVE;
    }

    TBSYS_LOG(DEBUG,"cmd=%s", cmd);
    if (0!=strncasecmp("ZRANGEBYSCORE ",cmd,14)) {
        TBSYS_LOG(DEBUG,"not ZRANGEBYSCORE");
        return FAILED_ACTIVE;
    }

    reply=(redisReply* )redisCommand(rc->redis_context_,cmd);
    if (NULL==reply) {
        TBSYS_LOG(DEBUG,"NULL==reply");
        return FAILED_NOT_ACTIVE;
    }
    TBSYS_LOG(DEBUG,"type=%d,integer=%d,elements=%d",
        reply->type, reply->integer, reply->elements);

    int ret=FAILED_ACTIVE;
    if (REDIS_REPLY_ARRAY==reply->type && reply->elements>0) {
        ret=SUCCESS_ACTIVE;
    }

    if (free_reply || FAILED_ACTIVE==ret) {
        freeReplyObject(reply);
    }
    TBSYS_LOG(DEBUG,"ret=%d",ret);
    return ret;
}

int Rzcard(RedisClient* rc, const char* cmd, 
redisReply*& reply, bool free_reply) {
    if (NULL==rc) {
        TBSYS_LOG(DEBUG,"NULL==rc");
        return FAILED_NOT_ACTIVE;
    }

    if (NULL==rc->redis_context_) {
        TBSYS_LOG(DEBUG,"NULL==rc->redis_context_");
        return FAILED_NOT_ACTIVE;
    }

    TBSYS_LOG(DEBUG,"cmd=%s", cmd);
    if (0!=strncasecmp("ZCARD ",cmd,6)) {
        TBSYS_LOG(DEBUG,"not ZCARD");
        return FAILED_ACTIVE;
    }

    reply=(redisReply* )redisCommand(rc->redis_context_,cmd);
    if (NULL==reply) {
        TBSYS_LOG(DEBUG,"NULL==reply");
        return FAILED_NOT_ACTIVE;
    }
    TBSYS_LOG(DEBUG,"type=%d,integer=%d,elements=%d",
        reply->type, reply->integer, reply->elements);

    int ret=FAILED_ACTIVE;
    if (REDIS_REPLY_INTEGER==reply->type 
        && reply->integer > 0) {
        ret=SUCCESS_ACTIVE;
    }

    if (free_reply || FAILED_ACTIVE==ret) {
        freeReplyObject(reply);
    }
    TBSYS_LOG(DEBUG,"ret=%d",ret);
    return ret;
}



}
}