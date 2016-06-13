#include "shared.h"
#include "handler.h"

namespace lljz {
namespace disk {

void ModifyLoginPasswordReq(RequestPacket* req,
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

    //检查old_password
    if (!req_doc.HasMember("old_password") 
        || !req_doc["old_password"].IsString()
        || 0==req_doc["old_password"].GetStringLength()) {
        SetErrorMsg(20001,"old_password is invalid",resp);
        return;
    }
    
    //检查new_password
    if (!req_doc.HasMember("new_password") 
        || !req_doc["new_password"].IsString()
        || 0==req_doc["new_password"].GetStringLength()) {
        SetErrorMsg(20001,"new_password is invalid",resp);
        return;
    }

    //判断账号是否存在
    RedisClient* rc=g_account_redis->GetRedisClient();
    char cmd[512];
    sprintf(cmd,"EXISTS %s", req_doc["account"].GetString());
    redisReply* reply;
    int cmd_ret=Rexists(rc,cmd,reply);
    if (FAILED_NOT_ACTIVE==cmd_ret) {
        g_account_redis->ReleaseRedisClient(rc,false);
        SetErrorMsg(20002,"redis database server is busy",resp);
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {
        g_account_redis->ReleaseRedisClient(rc,true);
        SetErrorMsg(20003,"account does not exist",resp);
        return;
    }

    sprintf(cmd,"HGET %s password", req_doc["account"].GetString());
    cmd_ret=Rhget(rc,cmd,reply,false);
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        g_account_redis->ReleaseRedisClient(rc,false);
        SetErrorMsg(20002,"redis database server is busy",resp);
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {//没有字段password，允许直接设置密码
    } else {
        if (0!=strcmp(req_doc["old_password"].GetString(),reply->str)) {
            g_account_redis->ReleaseRedisClient(rc,true);
            SetErrorMsg(20005,"login password is wrong",resp);
            freeReplyObject(reply);
            return;
        }
    }
    freeReplyObject(reply);
    if (0==strcmp(req_doc["old_password"].GetString(),
                req_doc["new_password"].GetString()) ) {//新密码不变
        g_account_redis->ReleaseRedisClient(rc,true);
        SetErrorMsg(0,"",resp);
        return;
    }

    sprintf(cmd,"HSET %s password %s",
        req_doc["account"].GetString(),
        req_doc["new_password"].GetString());
    cmd_ret=Rhset(rc,cmd,reply);
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        g_account_redis->ReleaseRedisClient(rc,false);
        SetErrorMsg(20006,"modify login password failed,redis database server is busy",resp);
        return;
    } else if (FAILED_ACTIVE == cmd_ret) {
        g_account_redis->ReleaseRedisClient(rc,true);
        SetErrorMsg(20006,"modify login password fail",resp);
        return;
    }

    g_account_redis->ReleaseRedisClient(rc,true);
    SetErrorMsg(0,"",resp);
}

}
}