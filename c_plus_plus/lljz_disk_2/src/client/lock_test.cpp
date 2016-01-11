#include "tbnet.h"
#include <random>

int main(int argc, char *argv[])
{
    int i;
    int n=10000000;
    int64_t start_time,end_time;
    start_time=tbsys::CTimeUtil::getTime();
    for (i=0;i<n;i++) {

    }
    end_time=tbsys::CTimeUtil::getTime();
    printf("test i++:n=%d,time=%lld\n",n,end_time-start_time);

    std::random_device rd;
    std::default_random_engine e(rd());
    std::uniform_int_distribution<> u(0, 15);
    start_time=tbsys::CTimeUtil::getTime();
    for (i=0;i<500;i++) {
        printf("%d ",u(e));
    }
    end_time=tbsys::CTimeUtil::getTime();
    printf("test random:n=%d,time=%lld\n",n,end_time-start_time);

    tbsys::CThreadMutex mutex;
    start_time=tbsys::CTimeUtil::getTime();
    for (i=0;i<n;i++) {
        mutex.lock();
        mutex.unlock();
    }
    end_time=tbsys::CTimeUtil::getTime();
    printf("test lock:n=%d,time=%lld\n",n,end_time-start_time);

    tbsys::CRWSimpleLock rwlock;
    start_time=tbsys::CTimeUtil::getTime();
    for (i=0;i<n;i++) {
        rwlock.rdlock();
        rwlock.unlock();
    }
    end_time=tbsys::CTimeUtil::getTime();
    printf("test rd lock:n=%d,time=%lld\n",n,end_time-start_time);

    start_time=tbsys::CTimeUtil::getTime();
    for (i=0;i<n;i++) {
        rwlock.wrlock();
        rwlock.unlock();
    }
    end_time=tbsys::CTimeUtil::getTime();
    printf("test wr lock:n=%d,time=%lld\n",n,end_time-start_time);

    __gnu_cxx::hash_map<int,int> imap;
    start_time=tbsys::CTimeUtil::getTime();
    for (i=0;i<n;i++) {
        imap[i]=i;
    }
    end_time=tbsys::CTimeUtil::getTime();
    printf("test insert map:n=%d,time=%lld\n",n,end_time-start_time);

    start_time=tbsys::CTimeUtil::getTime();
    for (i=0;i<n;i++) {
        imap.find(i);
    }
    end_time=tbsys::CTimeUtil::getTime();
    printf("test find map:n=%d,time=%lld\n",n,end_time-start_time);

    imap.clear();
    n=3*n;
    start_time=tbsys::CTimeUtil::getTime();
    for (i=0;i<n;i++) {
        imap[i]=i;
    }
    end_time=tbsys::CTimeUtil::getTime();
    printf("test insert map:n=%d,time=%lld\n",n,end_time-start_time);

    start_time=tbsys::CTimeUtil::getTime();
    for (i=0;i<n;i++) {
        imap.find(i);
    }
    end_time=tbsys::CTimeUtil::getTime();
    printf("test find map:n=%d,time=%lld\n",n,end_time-start_time);

    return EXIT_SUCCESS;
}


