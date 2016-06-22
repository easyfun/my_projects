#include "handler.h"
#include "shared.h"

namespace lljz {
namespace disk {

/**************************************
 *global
**************************************/
RedisClientManager* g_account_redis=NULL;
RedisClientManager* g_file_redis=NULL;

RSA* g_serv_pri_rsa=0;
int g_serv_pri_rsa_len=0;
static unsigned char g_blowfish_cbc_iv[8]=
    {0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};

UserManager* g_userManager=0;

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

    FILE *f_serv_pri_key=fopen("pri.key","r");
    if(0==f_serv_pri_key){
        TBSYS_LOG(ERROR,"%s","open pri.key failed");
        return false;
    }
    g_serv_pri_rsa=PEM_read_RSAPrivateKey(f_serv_pri_key,NULL,NULL,NULL);
    if(0==g_serv_pri_rsa){
        TBSYS_LOG(ERROR,"%s","read pri.key failed");
        fclose(f_serv_pri_key);
        return false;
    }
    g_serv_pri_rsa_len=RSA_size(g_serv_pri_rsa);
    fclose(f_serv_pri_key);

    g_userManager=new UserManager();
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

    if (g_serv_pri_rsa){
        RSA_free(g_serv_pri_rsa);
    }

    if (g_userManager){
        delete g_userManager;
        g_userManager=0;
    }
}

void registerHandler() {
    HANDLER_ROUTER.RegisterHandler(
        PUBLIC_ECHO_TEST_REQ,
        publicEchoTestReqHandler);
    
    HANDLER_ROUTER.RegisterHandler(
        ACCOUNT_SERVER_APP_LOGIN_REQ, 
        appLoginReq);

    HANDLER_ROUTER.RegisterHandler(
        ACCOUNT_SERVER_REGISTER_ACCOUNT_REQ,
        registerAccountReq);
    
    HANDLER_ROUTER.RegisterHandler(
        ACCOUNT_SERVER_SET_ACCOUNT_INFO_REQ,
        setAccountInfoReq);
    
    HANDLER_ROUTER.RegisterHandler(
        ACCOUNT_SERVER_MODIFY_LOGIN_PASSWORD_REQ,
        modifyLoginPasswordReq);
    
    HANDLER_ROUTER.RegisterHandler(
        ACCOUNT_SERVER_LOGIN_REQ,
        loginReq);
    
    HANDLER_ROUTER.RegisterHandler(
        ACCOUNT_SERVER_LOGOUT_REQ,
        logoutReq);
}

}
}