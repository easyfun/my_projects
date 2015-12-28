#include "account_server_handler.h"

namespace lljz {
namespace disk {

void SetAccountInfoReq(RequestPacket* req,
void* args, ResponsePacket* resp) {
    Document req_doc;
/*    Document resp_doc;
    Document::AllocatorType& resp_allocator=resp_doc.GetAllocator();
    StringBuffer resp_buffer;
    Writer<StringBuffer> resp_writer(resp_buffer);
    Value resp_json(kObjectType);
    Value resp_error_msg(kStringType);
*/
    std::string resp_data;

    TBSYS_LOG(DEBUG,"-------data:%s",req->data_);
    req_doc.Parse(req->data_);

    //检查account
    if (!req_doc.HasMember("account") 
        || !req_doc["account"].IsString()
        || 0==req_doc["account"].GetStringLength()) {
        SetErrorMsg(20001,"account is invalid",resp);
        return;
    }

    //检查img_url
    if (req_doc.HasMember("img_url")) {
        if (!req_doc["img_url"].IsString()) {
            SetErrorMsg(20001,"img_url is invalid",resp);
            return;
        }
    }
    
    //检查home_page
    if (req_doc.HasMember("home_page")) {
        if (!req_doc["home_page"].IsString()) {
            SetErrorMsg(20001,"home_page is invalid",resp);
            return;
        }
    }

    if (0==req_doc["img_url"].GetStringLength() && 
        0==req_doc["home_page"].GetStringLength()) {
        SetErrorMsg(0,"modify nothing",resp);
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
        SetErrorMsg(20003,"redis database server is busy",resp);
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {
        g_account_redis->ReleaseRedisClient(rc,true);
        SetErrorMsg(20003,"account does not exist",resp);
        return;
    }

    sprintf(cmd,"HMSET %s",req_doc["account"].GetString());
    char fv[512];
    if (req_doc["img_url"].GetStringLength()) {
        sprintf(fv," img_url %s",req_doc["img_url"].GetString());
        strcat(cmd,fv);
    }

    if (req_doc["home_page"].GetStringLength()) {
        sprintf(fv," home_page %s",req_doc["home_page"].GetString());
        strcat(cmd,fv);
    }

    sprintf(fv," timestamp %lld",tbsys::CTimeUtil::getTime());
    strcat(cmd,fv);
    cmd_ret=Rhmset(rc,cmd,reply);
    if (FAILED_NOT_ACTIVE==cmd_ret) {
        g_account_redis->ReleaseRedisClient(rc,false);
        SetErrorMsg(20004,"set account info fail,redis database server is busy",resp);
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {
        g_account_redis->ReleaseRedisClient(rc,true);
        SetErrorMsg(20004,"set account info fail",resp);
        return;
    }
    g_account_redis->ReleaseRedisClient(rc,true);
    SetErrorMsg(0,"",resp);
}

}
}