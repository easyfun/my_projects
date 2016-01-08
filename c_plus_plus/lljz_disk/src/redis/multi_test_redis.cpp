#include "tbsys.h"
#include "tbnet.h"
#include "hiredis.h"
#include "redis_client_manager.h"
#include "redis_client.h"

class TestMain:public tbsys::Runnable {
public:
    TestMain() {
        all_count=fail_count=suc_count=0;
    };
    ~TestMain() {};

    void start() {
        test_thread_num=TBSYS_CONFIG.getInt("redis","test_thread_num",8);
        if (test_thread_num<=0) {
            test_thread_num=8;
        }
        TBSYS_LOG(DEBUG,"test_thread_num=%d",test_thread_num);

        redis_port=TBSYS_CONFIG.getInt("redis","port",6379);
        TBSYS_LOG(DEBUG,"redis_port=%d",redis_port);
        
        const char* str_config_value=
            TBSYS_CONFIG.getString("redis","host","");
        sprintf(redis_host,"%s",str_config_value);
        TBSYS_LOG(DEBUG,"redis_host=%s",redis_host);
        start_time=tbsys::CTimeUtil::getTime();

        timer_thread_=new tbsys::CThread;
        display_thread_.start(this,NULL);
        for (int i=0;i<test_thread_num;i++) {
            timer_thread_[i].start(this,NULL);
        }
    }

    void stop() {

    }

    void wait() {
        for (int i=0;i<test_thread_num;i++) {
            timer_thread_[i].join();
        }
        display_thread_.join();
    }

    //Runnable
    void run(tbsys::CThread* thread, void* arg) {
        if (thread==&display_thread_) {
            display();
        } else {
            test_redis_set();
        }
    }

    void display() {
        int64_t use_time=0;
        int64_t tps=0;

        while (1) {
            end_time=tbsys::CTimeUtil::getTime();
            //mutex_.lock();
            use_time=end_time-start_time;
            tps=suc_count*1000000/use_time;
            printf("all_count=%lld, suc_count=%lld, fail_count=%lld,all_time=%lld, tps=%lld\n",
                all_count,suc_count,fail_count,use_time,tps);
            //mutex_.unlock();
            usleep(500000);
        }
    }

    redisContext* connect_to_redis() {
        redisContext* rc=NULL;
        struct timeval tv;
        tv.tv_sec=1000/1000;
        tv.tv_usec=1000*1000;
        rc=redisConnectWithTimeout(redis_host,redis_port,tv);
        return rc;
    }

    //
    void test_redis_set() {
        char cmd[200];
        int64_t index=0;
        redisReply* reply;
        int n=1;
        redisContext* rc=NULL;

        while (1) {
            for (int i=0;i<n;i++) {
                if (NULL==rc) {
                    rc=connect_to_redis();
                }
                sprintf(cmd,"set yjb_%lld %s",index++,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
                reply=(redisReply* )redisCommand(rc,cmd);

                mutex_.lock();
                all_count+=1;
                mutex_.unlock();

                if (NULL==reply) {
                    printf("%s","redis_client set fail, network error");
                    if (rc) {
                        redisFree(rc);
                    }
                    mutex_.lock();
                    fail_count+=1;
                    mutex_.unlock();
                    continue;
                }
                //printf("type=%d, str=%s\n", reply->type, reply->str);
                if( !(reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str,"OK")==0)) {
                    printf("%s","redis_client set fail");
                    freeReplyObject(reply);
                    mutex_.lock();
                    fail_count+=1;
                    mutex_.unlock();
                    continue;
                }

                mutex_.lock();
                suc_count+=1;
                mutex_.unlock();

                freeReplyObject(reply);
            }
            usleep(500);//500us
        }
    }

private:
    int test_thread_num;
    tbsys::CThread* timer_thread_;
    tbsys::CThread display_thread_;

    tbsys::CThreadMutex mutex_;

    char redis_host[100];
    int redis_port;

    int64_t suc_count;
    int64_t all_count;
    int64_t fail_count;

    int64_t start_time;
    int64_t end_time;

};


int main(int argc,char* argv[]) {
    if(TBSYS_CONFIG.load("test.ini")) {
        printf("load test.ini fail");
        return -1;
    }

    const char* sz_log_level = TBSYS_CONFIG.getString(
    "public", "log_level", "info");
    printf("sz_log_level=%s\n", sz_log_level);
    TBSYS_LOGGER.setLogLevel(sz_log_level);

    TestMain t;
    t.start();
    t.wait();
    return 0;
}