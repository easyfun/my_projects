#include "redis_client_manager.h"

namespace lljz {
namespace disk {

RedisClientManager::RedisClientManager() {
    stop_=false;
}

RedisClientManager::~RedisClientManager() {

}

bool RedisClientManager::start() {
    redis_client_num_=TBSYS_CONFIG.getInt("server","redis_client_num",0);
    if (0 >= redis_client_num_) {
        redis_client_num_=0;
        return true;
    }

    redis_client_pool_=new RedisClient[redis_client_num_];
    reconn_mutext_.lock();
    reconn_redis_client_.clear();
    for (int i=0;i<redis_client_num_;i++) {
        reconn_redis_client_.push(redis_client_pool_[i]);
    }
//    reconn_cond_.signal();
    reconn_mutext_.unlock();

    timer_thread_.start(this,NULL);
    stop_=false;
    return true;
}

bool RedisClientManager::stop() {
    stop_=true;
    return true;
}

bool RedisClientManager::wait() {
    timer_thread_.join();
    destroy();
}

void RedisClientManager::destroy() {

}

//Runnable
void RedisClientManager::run(tbsys::CThread* thread, void* arg) {
    int timer_check_reconn=0;//1s
    int timer_check_free=20;//30s

    while (!stop_) {
        if (0==timer_check_reconn) {
            CheckReconn();
        }
        if (0==timer_check_free) {
            CheckFree();
        }

        if (1==++timer_check_reconn) {
            timer_check_reconn=0;
        }

        if (30==++timer_check_free) {
            timer_check_free=0;
        }
        usleep(1000000);
    }
}

void RedisClientManager::CheckReconn() {
    std::queue<RedisClient*> copy;
    reconn_mutext_.lock();
    int size=reconn_redis_client_.size();
    for (int i=0;i<size;i++) {
        copy.push(reconn_redis_client_.pop());
    }
    reconn_mutext_.unlock();

    size=copy.size();
    for (int i=0;i<size;i++) {
        RedisClient* rc=copy.pop();
        if (rc->redis_context_) {
            redisFree(rc->redis_context_);
        }
        rc->redis_context_=
    }
}

void RedisClientManager::CheckFree() {
    int size;
    int64_t now_time=tbsys::CTimeUtil::getTime();

    std::queue<RedisClient*> copy;
    free_mutext_.lock();
    size=free_redis_client_.size();
    for (int i=0;i<size;i++) {
        copy.push(free_redis_client_.pop());
    }
    free_mutext_.unlock();

    size=copy.size();
    for (int i=0;i<size;i++) {
        RedisClient* rc=copy.pop();
        if (now_time - rc->last_used_time_ >= 30000000) {
            redisReply* reply=(redisReply* )redisCommand(
                rc->redis_context_,"ping");
            if (NULL==reply) {
                TBSYS_LOG(DEBUG,"%s","redis_client ping fail")
                reconn_mutext_.lock();
                reconn_redis_client_.push(rc);
                reconn_mutext_.unlock();
                continue;
            }

            if( !(reply->type == REDIS_REPLY_STATUS 
                    && strcasecmp(reply->str,"OK")==0)) {
                TBSYS_LOG(DEBUG,"%s","redis_client ping fail")
                freeReplyObject(reply);
                reconn_mutext_.lock();
                reconn_redis_client_.push(rc);
                reconn_mutext_.unlock();
                continue;
            }

            freeReplyObject(reply);
            rc->last_used_time_=tbsys::CTimeUtil::getTime();

            free_mutext_.lock();
            free_redis_client_.push(rc);
            free_mutext_.unlock();
        } else {
            free_mutext_.lock();
            free_redis_client_.push(rc);
            free_mutext_.unlock();
        }
    }
}


}
}