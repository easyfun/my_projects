#include "shared.h"
//#include "handler.h"

namespace lljz {
namespace disk {

/**************************************
 *global
**************************************/
RedisClientManager* g_account_redis=NULL;
RedisClientManager* g_file_redis=NULL;



/**************************************
 * module
**************************************/
bool initDatabase() {
    g_account_redis=new RedisClientManager();
    g_file_redis=new RedisClientManager();
    assert(g_account_redis);
    assert(g_file_redis);
    if (!g_account_redis->start("redis_account")) {
        return false;
    }

    if (!g_file_redis->start("redis_file")) {
        return false;
    }
    return true;
}

bool initHandler() {
    RegisterHandler();
    if (!InitDatabase()) {
        return false;
    }

    return true;
}

void waitHandler() {
    g_account_redis->wait();
    g_file_redis->wait();
}

void stopHandler() {
    if (g_account_redis) {
        g_account_redis->stop();
        delete g_account_redis;
    }

    if (g_file_redis) {
        g_file_redis->stop();
        delete g_file_redis;
    }
}

void registerHandler() {
    HANDLER_ROUTER.RegisterHandler(PUBLIC_ECHO_TEST_REQ,publicEchoTestReqHandler);
    HANDLER_ROUTER.RegisterHandler(ACCOUNT_SERVER_REGISTER_ACCOUNT_REQ,registerAccountReq);
    HANDLER_ROUTER.RegisterHandler(ACCOUNT_SERVER_SET_ACCOUNT_INFO_REQ,setAccountInfoReq);
    HANDLER_ROUTER.RegisterHandler(ACCOUNT_SERVER_MODIFY_LOGIN_PASSWORD_REQ,modifyLoginPasswordReq);
    HANDLER_ROUTER.RegisterHandler(ACCOUNT_SERVER_LOGIN_REQ,loginReq);
    HANDLER_ROUTER.RegisterHandler(ACCOUNT_SERVER_LOGOUT_REQ,logoutReq);
}

}
}