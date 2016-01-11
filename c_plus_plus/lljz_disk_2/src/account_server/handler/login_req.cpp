#include "shared.h"
#include "handler.h"

namespace lljz {
namespace disk {

void LoginReq(RequestPacket* req,
void* args, ResponsePacket* resp) {
    Document req_doc;
/*    Document resp_doc;
    Document::AllocatorType& resp_allocator=resp_doc.GetAllocator();
    StringBuffer resp_buffer;
    Writer<StringBuffer> resp_writer(resp_buffer);
    Value resp_json(kObjectType);
    Value resp_error_msg(kStringType);
*/
    TBSYS_LOG(DEBUG,"-------data:%s",req->data_);
    req_doc.Parse(req->data_);

    //检查account
    if (!req_doc.HasMember("account") 
        || !req_doc["account"].IsString()
        || 0==req_doc["account"].GetStringLength()) {
        SetErrorMsg(20001,"account is invalid",resp);
        return;
    }

    //检查password
    if (!req_doc.HasMember("password")
        || !req_doc["password"].IsString()
        || 0==req_doc["password"].GetStringLength()) {
        SetErrorMsg(20001,"password is invalid",resp);
        return;
    }
   
    RedisClient* rc=g_account_redis->GetRedisClient();
    char cmd[512];
    redisReply* reply;
    sprintf(cmd,"HGET %s password", req_doc["account"].GetString());
    int cmd_ret=Rhget(rc,cmd,reply,false);
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        g_account_redis->ReleaseRedisClient(rc,false);
        SetErrorMsg(20002,"redis database server is busy",resp);
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {
        //记录异常日志
        g_account_redis->ReleaseRedisClient(rc,true);
        SetErrorMsg(20002,"redis database server status error",resp);
        freeReplyObject(reply);
        return;
    }

    if (0!=strcmp(req_doc["password"].GetString(),reply->str)) {
        g_account_redis->ReleaseRedisClient(rc,true);
        SetErrorMsg(20005,"login password is wrong",resp);
        freeReplyObject(reply);
        return;
    }
    freeReplyObject(reply);

    g_account_redis->ReleaseRedisClient(rc,true);
    SetErrorMsg(0,"",resp);
}


}
}