#include "account_server_handler.h"

namespace lljz {
namespace disk {

void RegisterAccountReq(RequestPacket* req,
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

    //增加账号
    RedisClient* rc=g_account_redis->GetRedisClient();
    char cmd[200];
    int cmd_ret;

    sprintf(cmd,"HSETNX %s account %s", 
        req_doc["account"].GetString(),
        req_doc["password"].GetString());
    redisReply* reply;
    cmd_ret=Rhsetnx(rc,cmd,reply);
    if (FAILED_NOT_ACTIVE==cmd_ret) {
        g_account_redis->ReleaseRedisClient(rc,false);
        SetErrorMsg(20002,"redis database server is busy",resp);
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {
        g_account_redis->ReleaseRedisClient(rc,true);
        SetErrorMsg(20002,"account has existed",resp);
        return;
    }

    sprintf(cmd,"HMSET %s password %s timestamp %lld",
        req_doc["account"].GetString(),
        req_doc["password"].GetString(), 
        tbsys::CTimeUtil::getTime());
    cmd_ret=Rhmset(rc,cmd,reply);
    if (FAILED_NOT_ACTIVE==cmd_ret) {
        g_account_redis->ReleaseRedisClient(rc,false);
        SetErrorMsg(20002,"redis database server is busy",resp);
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {
        g_account_redis->ReleaseRedisClient(rc,true);
        SetErrorMsg(20002,"account has existed",resp);
        return;
    }
    g_account_redis->ReleaseRedisClient(rc,true);

    //增加账号文件夹Hash表,使用file_redis
    //为新文件夹增加hash表
    sprintf(cmd,"HSETNX folder_%s %c_create_time %lld",
        req_doc["account"].GetString(), 
        0x02, 
        tbsys::CTimeUtil::getTime());
    RedisClient* file_rc=g_file_redis->GetRedisClient();
    cmd_ret=Rhsetnx(file_rc,cmd,reply);
    if (FAILED_NOT_ACTIVE==cmd_ret) {
        g_file_redis->ReleaseRedisClient(file_rc,false);
        SetErrorMsg(20007,"create account root folder fail,redis database server is busy",resp);
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {
        g_file_redis->ReleaseRedisClient(file_rc,true);
        SetErrorMsg(20007,"create account root folder fail",resp);
        return;
    }

    //增加账号file_id_set
    sprintf(cmd,"SUNIONSTORE file_id_sets_%s %s",
        req_doc["account"].GetString(), 
        "global_file_id_sets");
    cmd_ret=Rsunionstore(file_rc, cmd, reply);
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        g_file_redis->ReleaseRedisClient(file_rc,false);
        SetErrorMsg(20007,"create file_id_set fail,redis database server is busy",resp);
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {
        //记录异常日志
        g_file_redis->ReleaseRedisClient(file_rc,true);
        SetErrorMsg(20007,"create file_id_set fail",resp);
        return;
    }
    g_file_redis->ReleaseRedisClient(file_rc,true);

    SetErrorMsg(0,"",resp);
}


}
}
