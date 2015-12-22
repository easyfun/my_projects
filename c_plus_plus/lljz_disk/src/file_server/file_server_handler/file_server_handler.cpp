#include "file_server_handler.h"

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
bool InitDatabase() {
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

bool InitHandler() {
    RegisterHandler();
    if (!InitDatabase()) {
        return false;
    }

    return true;
}

void WaitHandler() {
    g_account_redis->wait();
    g_file_redis->wait();
}

void StopHandler() {
    if (g_account_redis) {
        g_account_redis->stop();
        delete g_account_redis;
    }

    if (g_file_redis) {
        g_file_redis->stop();
        delete g_file_redis;
    }
}

void RegisterHandler() {
    HANDLER_ROUTER.RegisterHandler(PUBLIC_ECHO_TEST_REQ,PublicEchoTestReqHandler);
    HANDLER_ROUTER.RegisterHandler(FILE_SERVER_CREATE_FOLDER_REQ,CreateFolderReq);
/*    HANDLER_ROUTER.RegisterHandler(FILE_SERVER_MODIFY_PROPERTY_REQ,ModifyPropertyReq);
    HANDLER_ROUTER.RegisterHandler(FILE_SERVER_UPLOAD_FILE_REQ,UploadFileReq);
    HANDLER_ROUTER.RegisterHandler(FILE_SERVER_DOWNLOAD_FILE_REQ,DownloadFileReq);
    HANDLER_ROUTER.RegisterHandler(FILE_SERVER_DELETE_FILE_OR_FOLDER_REQ,DeleteFileOrFolderReq);
    */
}

}
}
