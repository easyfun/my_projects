#include "account_server_handler.h"

namespace lljz {
namespace disk {

void RegisterAccountReq(RequestPacket* req,
void* args, ResponsePacket* resp) {
    Document req_doc;
    Document resp_doc;
    Document::AllocatorType& resp_allocator=resp_doc.GetAllocator();
    StringBuffer resp_buffer;
    Writer<StringBuffer> resp_writer(resp_buffer);
    Value resp_json(kObjectType);
    Value resp_error_msg(kStringType);

    TBSYS_LOG(DEBUG,"-------data:%s",req->data_);
    req_doc.Parse(req->data_);
    std::string req_account;
    std::string req_password;

    //检查account
    if (!req_doc.HasMember("account") || !req_doc["account"].IsString()) {
        resp->error_code_=20001;
        resp_error_msg="account is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }
    req_account=req_doc["account"].GetString();

    //检查password
    if (!req_doc.HasMember("password") || !req_doc["password"].IsString() ) {
        resp->error_code_=20001;
        resp_error_msg="password is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }
    req_password=req_doc["password"].GetString();

    if (req_account.empty() || req_password.empty()) {
        resp->error_code_=20001;
        resp_error_msg="account,password is invalid";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }

    //增加账号
    RedisClient* rc=g_account_redis->GetRedisClient();
    char cmd[200];
    int cmd_ret;

    sprintf(cmd,"HSETNX %s account %s", 
        req_account.c_str(), req_account.c_str());
    redisReply* reply;
    cmd_ret=Rhsetnx(rc,cmd,reply);
    if (SUCCESS_ACTIVE != cmd_ret) {
        g_account_redis->ReleaseRedisClient(rc,cmd_ret);

        resp->error_code_=20002;
        resp_error_msg="redis database server is busy";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }

    sprintf(cmd,"HMSET %s password %s timestamp %lld",
        req_account.c_str(),req_password.c_str(), 
        tbsys::CTimeUtil::getTime());
    cmd_ret=Rhmset(rc,cmd,reply);
    if (SUCCESS_ACTIVE != cmd_ret) {
        g_account_redis->ReleaseRedisClient(rc,cmd_ret);

        resp->error_code_=20002;
        resp_error_msg="redis database server is busy";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
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
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        g_file_redis->ReleaseRedisClient(file_rc,false);
        resp->error_code_=20002;
        resp_error_msg="create account root folder fail,redis database server is busy";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {
        //记录异常日志
        g_file_redis->ReleaseRedisClient(file_rc,true);
        resp->error_code_=20007;
        resp_error_msg="create account root folder fail,redis database server status error";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }
    //增加账号file_id_set
    sprintf(cmd,"SUNIONSTORE file_id_sets_%s %s",
        req_doc["account"].GetString(), 
        "global_file_id_sets");
    cmd_ret=Rsunionstore(file_rc, cmd, reply);
    if (FAILED_NOT_ACTIVE==cmd_ret) {//网络错误
        g_file_redis->ReleaseRedisClient(file_rc,false);
        resp->error_code_=20002;
        resp_error_msg="create file_id_set fail,redis database server is busy";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    } else if (FAILED_ACTIVE==cmd_ret) {
        //记录异常日志
        g_file_redis->ReleaseRedisClient(file_rc,true);
        resp->error_code_=20007;
        resp_error_msg="create file_id_set fail,redis database server status error";
        resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

        resp_json.Accept(resp_writer);
        sprintf(resp->data_,"%s",resp_buffer.GetString());
        return;
    }
    g_file_redis->ReleaseRedisClient(file_rc,true);


    resp->error_code_=0;
    resp_error_msg="";
    resp_json.AddMember("error_msg",resp_error_msg,resp_allocator);

    resp_json.Accept(resp_writer);
    sprintf(resp->data_,"%s",resp_buffer.GetString());
}


}
}
