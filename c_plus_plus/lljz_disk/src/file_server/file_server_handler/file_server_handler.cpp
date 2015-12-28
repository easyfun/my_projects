#include "file_server_handler.h"

namespace lljz {
namespace disk {

/**************************************
 *global
**************************************/
RedisClientManager* g_account_redis=NULL;
RedisClientManager* g_file_redis=NULL;
TfsClient* g_tfs_client=NULL;


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

    const char* nsip=TBSYS_CONFIG.getString("tfs_name_server",
        "name_server_spec","");
    TBSYS_LOG(DEBUG,"------------nsip=%s",nsip);
    g_tfs_client=TfsClient::Instance();
    if (TFS_SUCCESS != g_tfs_client->initialize(nsip)) {
        TBSYS_LOG(ERROR,"%s","TfsClient::initialize fail");
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
    HANDLER_ROUTER.RegisterHandler(FILE_SERVER_MODIFY_PROPERTY_REQ,ModifyPropertyReq);
    HANDLER_ROUTER.RegisterHandler(FILE_SERVER_UPLOAD_FILE_REQ,UploadFileReq);
    HANDLER_ROUTER.RegisterHandler(FILE_SERVER_DOWNLOAD_FILE_REQ,DownloadFileReq);
    HANDLER_ROUTER.RegisterHandler(FILE_SERVER_DOWNLOAD_FILE_GET_INFO_REQ,DownloadFileGetInfoReq);
    //HANDLER_ROUTER.RegisterHandler(FILE_SERVER_DOWNLOAD_FILE_BY_SEQ_NO_REQ,DownloadFileBySeqNoReq);
/*    HANDLER_ROUTER.RegisterHandler(FILE_SERVER_DELETE_FILE_OR_FOLDER_REQ,DeleteFileOrFolderReq);
    */
}


/**************************************
 * 
**************************************/
bool CheckAuth(RequestPacket* req,ResponsePacket* resp) {
    Document req_doc;
    req_doc.Parse(req->data_);
    //检查account
    if (!req_doc.HasMember("account") 
        || !req_doc["account"].IsString() 
        || !req_doc["account"].GetStringLength()) {
        SetErrorMsg(35001,"account is invalid",resp);
        return false;
    }

    //检查password
    if (!req_doc.HasMember("password") 
        || !req_doc["password"].IsString() 
        || !req_doc["password"].GetStringLength()) {
        SetErrorMsg(35001,"password is invalid",resp);
        return false;
    }

    RedisClient* rc=g_account_redis->GetRedisClient();
    char cmd[512];
    redisReply* reply;
    int cmd_ret;
    //检查认证
    sprintf(cmd,"HGET %s password", req_doc["account"].GetString());
    cmd_ret=Rhget(rc,cmd,reply,false);
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        SetErrorMsg(35002,"redis database server is busy",resp);
        return false;
    } else if (FAILED_ACTIVE==cmd_ret) {//没有字段password
        //记录异常日志
        freeReplyObject(reply);
        g_account_redis->ReleaseRedisClient(rc,cmd_ret);
        SetErrorMsg(35002,"redis database server status error",resp);
        return false;
    }

    if (0!=strcmp(req_doc["password"].GetString(),reply->str)) {
        freeReplyObject(reply);
        g_account_redis->ReleaseRedisClient(rc,cmd_ret);
        SetErrorMsg(35007,"login password is wrong",resp);
        return false;
    }
    freeReplyObject(reply);
    g_account_redis->ReleaseRedisClient(rc,true);
    return true;
}

bool WriteFileToTFS(const char* data, int len, 
char* tfs_file_name) {
    int ret = 0;
    int fd = -1;

    // 创建tfs客户端，并打开一个新的文件准备写入 
    fd = g_tfs_client->open((char*)NULL, NULL, NULL, T_WRITE);
    if (fd <= 0) {
        TBSYS_LOG(ERROR,"create remote file error!");
        return false;
    }
    
    int wrote = 0;
    int left = len;
    while (left > 0) {
        // 将buffer中的数据写入tfs
        ret = g_tfs_client->write(fd, data + wrote, left);
        if (ret < 0 || ret >= left) {// 读写失败或完成
            break;
        } else {// 若ret>0，则ret为实际写入的数据量
            wrote += ret;
            left -= ret;
        }
    }

    // 读写失败   
    if (ret < 0) {
        TBSYS_LOG(ERROR,"write data error!");
        g_tfs_client->close(fd);
        return false;
    }

    // 提交写入 
    ret = g_tfs_client->close(fd, tfs_file_name, TFS_FILE_LEN);
    if (ret != TFS_SUCCESS) {// 提交失败
        TBSYS_LOG(ERROR,"write remote file failed! ret: %s", ret);
        return false;
    } 
    return true;
}

bool DownloadFileFromTFS(const char* tfs_file_name,
char* data,int& len) {
    int ret = 0;
    int fd = -1;

    // 打开待读写的文件
    fd = g_tfs_client->open(tfs_file_name, NULL, T_READ);
    if (ret != TFS_SUCCESS) {
        TBSYS_LOG(ERROR,"open remote file %s error", tfs_file_name);
        return false;
    }

    // 获得文件属性
    TfsFileStat fstat;
    ret = g_tfs_client->fstat(fd, &fstat);
    if (ret != TFS_SUCCESS || fstat.size_ <= 0) {
        TBSYS_LOG(ERROR,"get remote file info error");
        g_tfs_client->close(fd);
        return false;
    }

    if (fstat.size_ > 25165824) {//24KB
        TBSYS_LOG(ERROR,"remote file size more than 24KB,fatal error");
        g_tfs_client->close(fd);
        return false;
    } else if (fstat.size_ > len) {//24KB
        TBSYS_LOG(ERROR,"the buffer is small");
        g_tfs_client->close(fd);
        return false;
    }
    len=fstat.size_;
    
    int read = 0;
    uint32_t crc = 0;
    
    // 读取文件
    while (read < fstat.size_) {
        ret = g_tfs_client->read(fd, data + read, fstat.size_ - read);
        if (ret < 0) {
            break;
        } else {
            crc = Func::crc(crc, data + read, ret); // 对读取的文件计算crc值
            read += ret;
        }
    }

    if (ret < 0 || crc != fstat.crc_) {
        TBSYS_LOG(ERROR,"read remote file error!");
        g_tfs_client->close(fd);
        return false;
    }

    ret = g_tfs_client->close(fd);
    if (ret < 0) {
        TBSYS_LOG(ERROR,"close remote file error!");
        return false;
    }
    return true;
}


}
}
