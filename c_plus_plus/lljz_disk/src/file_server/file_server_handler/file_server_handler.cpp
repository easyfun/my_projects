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
    HANDLER_ROUTER.RegisterHandler(FILE_SERVER_MODIFY_PROPERTY_REQ,ModifyPropertyReq);
/*    HANDLER_ROUTER.RegisterHandler(FILE_SERVER_UPLOAD_FILE_REQ,UploadFileReq);
    HANDLER_ROUTER.RegisterHandler(FILE_SERVER_DOWNLOAD_FILE_REQ,DownloadFileReq);
    HANDLER_ROUTER.RegisterHandler(FILE_SERVER_DELETE_FILE_OR_FOLDER_REQ,DeleteFileOrFolderReq);
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

void SetErrorMsg(uint32_t error_code, 
const char* error_msg, ResponsePacket* resp) {
    Document req_doc;
    Document resp_doc;
    Document::AllocatorType& resp_allocator=resp_doc.GetAllocator();
    StringBuffer resp_buffer;
    Writer<StringBuffer> resp_writer(resp_buffer);
    Value resp_json(kObjectType);
    Value resp_error_msg(kStringType);

    resp->error_code_=error_code;
    resp_error_msg.SetString(error_msg,strlen(error_msg),resp_allocator);
    resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

    resp_json.Accept(resp_writer);
    sprintf(resp->data_,"%s",resp_buffer.GetString());
}

int GetCharCount(const char* str, char c) {
    int n=0;
    while (*str) {
        if (*str==c)
            n++;
        str++;
    }
    return n;
}

//from 1
void GetStrValue(const char* str, char c,int index, char* value) {
    int n=0;
    const char* head;
    const char* tail;
    head=str;
    tail=NULL;
    value[0]='\0';
    while (*str) {
        if (*str++ != c) {
            continue;
        }
        n++;
        if (n==index-1) {
            head=str;
        } else if (n==index) {
            tail=str-1;
            break;
        } else if (n>index) {
            break;
        }
    }
    if (0==n) {
        tail=str;
    } else if (n < index) {
        if (n < index-1)
            head=str;
        tail=str;
    }
    if (tail) {
        memcpy(value,head,tail-head);
        value[tail-head]='\0';
    }
}

}
}
